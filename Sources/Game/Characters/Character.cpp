#include "Character.h"
#include "Player.h"
#include "box2d/box2d.h"
#include "raylib.h"
#include <iostream>
#include <Engine/GameObjects/PhysicsRectangle.h>
#include <Game/PhysicsCategory.h>

Character::Character()
{
    ragdollHeadTexture = Engine::LoadTextureFromTexturePool("Assets/Textures/CharacterRagDollHead.png");
    ragdollBodyTexture = Engine::LoadTextureFromTexturePool("Assets/Textures/CharacterRagDollBody.png");
    ragdollLegsTexture = Engine::LoadTextureFromTexturePool("Assets/Textures/CharacterRagDollLegs.png");
    lastFrameTime = GetTime();

    initPhysicsBody();
    SetDrawPriority(LOW);
    bloodParticlesLeft = rand() % 10 + rand() % 40;

    useBleedingBursts = rand() % 2 == 0;
    randomizeBleedBurstsValues();
}

Character::~Character()
{
}

void Character::randomizeBleedBurstsValues()
{
    bleedBurstsIntervalCur = 
        bleedBurstsIntervalMin + 
        static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(bleedBurstsIntervalMax-bleedBurstsIntervalMin)));

    bleedBurstCurParticlesLeft = bleedBurstMinBloodParticles + rand() % (bleedBurstMaxBloodParticles - bleedBurstMinBloodParticles);
}

void Character::Update()
{
    if (!Player::IsAlive)
        return;

    handleAudio();

    if (!dead)
    {
        // Progress animation
        if (currentTexture != nullptr && currentTexture->id != 0)
        {
            if (GetTime() - lastFrameTime > frameDuration)
            {
                if (repeatAnimation)
                {
                    curFrame = (curFrame + 1) % totalFrames;
                }
                else if (!animationPaused)
                {
                    if (curFrame < totalFrames - 1)
                        curFrame++;
                    else
                        animationPaused = true;
                }

                lastFrameTime = GetTime();
            }
        }

        // Get box2d result value
        b2Vec2 pos = b2Body_GetWorldPoint(physBodyId, { -extent.x, -extent.y });
        position = {pos.x, pos.y};
    }
    else
    {
        if (position.y > Engine::GetInternalResolution().y)
            QueueDestroy();
        else
        {
            if (bloodParticlesLeft > 0)
            {
                if (useBleedingBursts)
                {
                    if (GetTime() - lastBleedBurstTime > bleedBurstsIntervalCur)
                    {
                        if (bleedBurstCurParticlesLeft > 0 )
                        {
                            if (GetTime() - lastBleedTime > bleedInterval)
                            {
                                dropBloodParticle();                    
                                bleedBurstCurParticlesLeft--;
                            }
                        }
                        else
                        {
                            randomizeBleedBurstsValues();
                            lastBleedBurstTime = GetTime();
                        }
                    }
                }
                else
                {
                    if (GetTime() - lastBleedTime > bleedInterval)
                    {
                        dropBloodParticle();                    
                        bleedBurstCurParticlesLeft--;
                    }
                }
            } 

            b2Vec2 pos = b2Body_GetWorldPoint(ragdollBodyId, { -extent.x, -extent.y });
            position = {pos.x, pos.y};
            
            if (destroyTimeStart == 0)
                destroyTimeStart = GetTime();
            else
            {
                if (GetTime() - destroyTimeStart > destroyTime)
                {
                    Engine::SetPhysFilterCategories(ragdollHeadShapeId, 0, 0);
                    Engine::SetPhysFilterCategories(ragdollBodyShapeId, 0, 0);
                    Engine::SetPhysFilterCategories(ragdollLegsShapeId, 0, 0);
                }
            }
        }
    }

    prevFrame = curFrame;
}

void Character::Draw()
{    
    if (!dead)
    {
        if (currentTexture && currentTexture->id != 0)
        {            
            // DrawRectanglePro(
            //     {position.x,position.y, size.x, size.y},
            //     {0}, 
            //     RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(physBodyId)), 
            //     GREEN
            // );

            Rectangle sourceRect = {
                (float) (curFrame % curTextureCols) * size.x, 
                (float) (curFrame / curTextureCols) * size.y, 
                lookDirection.x < 0 ? -size.x : size.x, 
                size.y
            };

            DrawTexturePro(
                *currentTexture,
                sourceRect,
                {position.x, position.y, size.x, size.y},
                {0},
                RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(physBodyId)), 
                WHITE
            );
        }
        else
        {
            DrawRectanglePro(
                {position.x,position.y, size.x, size.y},
                {0}, 
                RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(physBodyId)), 
                WHITE
            );
        }
    }
    else
    {
        b2Vec2 headPos = b2Body_GetWorldPoint(ragdollHeadId, { -ragdollHeadExtent.x, -ragdollHeadExtent.y });
        b2Vec2 bodyPos = b2Body_GetWorldPoint(ragdollBodyId, { -ragdollBodyExtent.x, -ragdollBodyExtent.y });
        b2Vec2 legsPos = b2Body_GetWorldPoint(ragdollLegsId, { -ragdollLegsExtent.x, -ragdollLegsExtent.y });
        float headAngle = RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(ragdollHeadId));
        float bodyAngle = RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(ragdollBodyId));
        float legsAngle = RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(ragdollLegsId));

        DrawTexturePro(
            ragdollHeadTexture, 
            {0, 0, (lookDirection.x < 0 ? -1.0f : 1.0f) * ragdollHeadTexture.width, (float) ragdollHeadTexture.height}, 
            {headPos.x, headPos.y, (float) ragdollHeadTexture.width, (float) ragdollHeadTexture.height}, 
            {0}, 
            headAngle, 
            WHITE
        );

        DrawTexturePro(
            ragdollBodyTexture, 
            {0, 0, (lookDirection.x < 0 ? -1.0f : 1.0f) * ragdollBodyTexture.width, (float) ragdollBodyTexture.height}, 
            {bodyPos.x, bodyPos.y, (float) ragdollBodyTexture.width, (float) ragdollBodyTexture.height}, 
            {0}, 
            bodyAngle, 
            WHITE
        );

        DrawTexturePro(
            ragdollLegsTexture, 
            {0, 0, (lookDirection.x < 0 ? -1.0f : 1.0f) * ragdollLegsTexture.width, (float) ragdollLegsTexture.height}, 
            {legsPos.x, legsPos.y, (float) ragdollLegsTexture.width, (float) ragdollLegsTexture.height}, 
            {0}, 
            legsAngle, 
            WHITE
        );
    }
}

void Character::SetPosition(Vector2 pos)
{
    position = pos;
    b2Body_SetTransform(physBodyId, {pos.x, pos.y}, b2Body_GetRotation(physBodyId));
}

void Character::SetAnimationTexture(Texture2D *texture)
{
    currentTexture = texture;
    curTextureRows = texture->height / size.y;
    curTextureCols = texture->width / size.x;
    totalFrames = curTextureRows * curTextureCols;
    curFrame = 0;
    lastFrameTime = GetTime();
    animationPaused = false;
}

void Character::initPhysicsBody()
{
    b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_dynamicBody;
    bodyDef.fixedRotation = true;
    bodyDef.position = {position.x + (size.x * 0.5f), position.y + (size.y * 0.5f)};
	physBodyId = b2CreateBody( Engine::GetPhysWorldID(), &bodyDef );
    
    extent = { 0.5f * size.x, 0.5f * size.y };
    physBodyId = b2CreateBody(Engine::GetPhysWorldID(), &bodyDef);
    b2Polygon bodyCube = b2MakeBox(extent.x, extent.y);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 0.05;
    shapeDef.friction = 1;
    physShapeId = b2CreatePolygonShape(physBodyId, &shapeDef, &bodyCube);
    b2Shape_EnableHitEvents(physShapeId, true);

    physBodies.push_back(physBodyId);
    physShapes.push_back(physShapeId);
}


void Character::Die(Vector2 reactionDirection, float reactionForce, bool dismember)
{
    if (!dead)
    {
        dead = true;

        if (reactionDirection.x != 0 || reactionDirection.y != 0)
        {
            bleedDirection = {-reactionDirection.x, reactionDirection.y};
            b2Vec2 dir = {bleedDirection.x, bleedDirection.y};
            dir = b2Normalize(dir);
            bleedDirection.x = dir.x;
            bleedDirection.y = dir.y;
            bleedDirectionIsSet = true;
        }
        
        createRagdollBodies(reactionDirection, reactionForce, dismember);
        DestroyPhysBody(physBodyId);
    }
}

void Character::createRagdollBodies(Vector2 reactionDirection, float reactionForce, bool dismember)
{
    // Create bodies
    Vector2 drawPos = {position.x + (size.x * 0.5f), position.y};
    b2Vec2 headSize = {(float) ragdollHeadTexture.width, (float) ragdollHeadTexture.height};
    b2Vec2 bodySize = {(float) ragdollBodyTexture.width, (float) ragdollBodyTexture.height};
    b2Vec2 legsSize = {(float) ragdollLegsTexture.width, (float) ragdollLegsTexture.height};
    ragdollHeadExtent = {0.5f * headSize.x, 0.5f * headSize.y};
    ragdollBodyExtent = {0.5f * bodySize.x, 0.5f * bodySize.y};
    ragdollLegsExtent = {0.5f * legsSize.x, 0.5f * legsSize.y};

    b2BodyDef headBodyDef = b2DefaultBodyDef();
    headBodyDef.type = b2_dynamicBody;
    headBodyDef.position = {drawPos.x, drawPos.y};
    ragdollHeadId = b2CreateBody(Engine::GetPhysWorldID(), &headBodyDef);
    b2Polygon headBodyCube = b2MakeBox(ragdollHeadExtent.x, ragdollHeadExtent.y);
    b2ShapeDef headShapeDef = b2DefaultShapeDef();
    headShapeDef.friction = 10;
    ragdollHeadShapeId = b2CreatePolygonShape(ragdollHeadId, &headShapeDef, &headBodyCube);
    drawPos.y += headSize.y;

    b2BodyDef bodyBodyDef = b2DefaultBodyDef();
    bodyBodyDef.type = b2_dynamicBody;
    bodyBodyDef.position = {drawPos.x, drawPos.y};
    ragdollBodyId = b2CreateBody(Engine::GetPhysWorldID(), &bodyBodyDef);
    b2Polygon bodyBodyCube = b2MakeBox(ragdollBodyExtent.x, ragdollBodyExtent.y);
    b2ShapeDef bodyShapeDef = b2DefaultShapeDef();
    bodyShapeDef.friction = 10;
    ragdollBodyShapeId = b2CreatePolygonShape(ragdollBodyId, &bodyShapeDef, &bodyBodyCube);
    drawPos.y += bodySize.y;

    b2BodyDef legsBodyDef = b2DefaultBodyDef();
    legsBodyDef.type = b2_dynamicBody;
    legsBodyDef.position = {drawPos.x, drawPos.y};
    ragdollLegsId = b2CreateBody(Engine::GetPhysWorldID(), &legsBodyDef);
    b2Polygon legsBodyCube = b2MakeBox(1, ragdollLegsExtent.y);
    b2ShapeDef legsShapeDef = b2DefaultShapeDef();
    legsShapeDef.friction = 10;
    ragdollLegsShapeId = b2CreatePolygonShape(ragdollLegsId, &legsShapeDef, &legsBodyCube);

    physBodies.push_back(ragdollBodyId);
    physBodies.push_back(ragdollHeadId);
    physBodies.push_back(ragdollLegsId);
    physShapes.push_back(ragdollBodyShapeId);
    physShapes.push_back(ragdollHeadShapeId);
    physShapes.push_back(ragdollLegsShapeId);
    
    // Setup joints
    if (!dismember)
    {
        b2RevoluteJointDef headJointDef = b2DefaultRevoluteJointDef();
        headJointDef.bodyIdA = ragdollHeadId;
        headJointDef.bodyIdB = ragdollBodyId;
        headJointDef.localAnchorA = { 0, ragdollHeadExtent.y };
        headJointDef.localAnchorB = { 0, -ragdollBodyExtent.y };
        headJointDef.lowerAngle = (-b2_pi/2);
        headJointDef.upperAngle = (b2_pi/2);
        headJointDef.enableLimit = true;
        b2CreateRevoluteJoint(Engine::GetPhysWorldID(), &headJointDef);

        b2RevoluteJointDef bodyJointDef = b2DefaultRevoluteJointDef();
        bodyJointDef.bodyIdA = ragdollBodyId;
        bodyJointDef.bodyIdB = ragdollLegsId;
        bodyJointDef.localAnchorA = { 0, ragdollBodyExtent.y };
        bodyJointDef.localAnchorB = { 0, -ragdollLegsExtent.y };
        bodyJointDef.lowerAngle = (-b2_pi/2);
        bodyJointDef.upperAngle = (b2_pi);
        bodyJointDef.enableLimit = true;
        b2CreateRevoluteJoint(Engine::GetPhysWorldID(), &bodyJointDef);
    }

    // Setup physics filters
    uint64_t ragdollFilterCategories = GamePhysicsCategories::BODY;
    uint64_t ragdollMaskCategories = GamePhysicsCategories::BODY | GamePhysicsCategories::GROUND | GamePhysicsCategories::TOWER_BRICK;
    Engine::SetPhysFilterCategories(ragdollHeadShapeId, ragdollFilterCategories, ragdollMaskCategories);
    Engine::SetPhysFilterCategories(ragdollBodyShapeId, ragdollFilterCategories, ragdollMaskCategories);
    Engine::SetPhysFilterCategories(ragdollLegsShapeId, ragdollFilterCategories, ragdollMaskCategories);

    // Apply same velocity to bodies as physBodyId
    if (reactionDirection.x != 0 || reactionDirection.y != 0)
    {
        b2Vec2 velocity = {reactionDirection.x * reactionForce, reactionDirection.y * reactionForce};
        b2Body_ApplyLinearImpulseToCenter(ragdollBodyId, velocity, true);

        if (dismember)
        {
            b2Body_ApplyLinearImpulseToCenter(ragdollHeadId, velocity * ((rand() % 15) / 10.0f), true);
            b2Body_ApplyLinearImpulseToCenter(ragdollLegsId, velocity * ((rand() % 15) / 10.0f), true);
        }
    }
    else
    {
        b2Vec2 velocity = b2Body_GetLinearVelocity(physBodyId);
        velocity.x *= -25 - (rand() % 5);
        velocity.y = velocity.x;
        velocity.x = lookDirection.x < 0 ? velocity.x : -velocity.x;
        b2Body_ApplyLinearImpulseToCenter(ragdollBodyId, velocity, true);

        if (dismember)
        {
            b2Body_ApplyLinearImpulseToCenter(ragdollHeadId, velocity * ((rand() % 15) / 10.0f), true);
            b2Body_ApplyLinearImpulseToCenter(ragdollLegsId, velocity * ((rand() % 15) / 10.0f), true);
        }
    }
}

void Character::dropBloodParticle()
{
    b2Vec2 pos = b2Body_GetPosition(ragdollBodyId);

    PhysicsRectangle *piece = new PhysicsRectangle(
        {pos.x, pos.y - 3},
        {1, 1}, 
        PhysicsRectangle::BodyType::DYNAMIC
    );
    
    b2Vec2 force = {(lookDirection.x < 0 ? 1.0f : -1.0f) * (rand() % 25 - 50), rand() % 2 - 4.0f};
    int multiplier = rand() % 25 + 50;
    
    if (bleedDirectionIsSet)
        force = {bleedDirection.x * multiplier * (lookDirection.x < 0 ? -1 : 1), bleedDirection.y * multiplier * -1};

    force = b2RotateVector(b2Body_GetRotation(ragdollBodyId), force);
    piece->SetColor(BLACK);

    Engine::SetPhysFilterCategories(piece->GetShapeId(), 0, 0);
    b2Body_ApplyLinearImpulseToCenter(piece->GetBodyId(), force, true);
    bloodParticlesLeft--;
    bleedInterval = 1.0 / (rand() % 1 + 23);
    lastBleedTime = GetTime();

    // Engine::PlayAudio("Bleed");
}

void Character::handleAudio()
{
    if (!dead)
    {
        if (playStepsSounds)
        {
            if (GetTime() - lastStepTime > stepsInterval)
            {
                lastStepTime = GetTime();

                if (isPlayer)
                    Engine::PlayAudio("StepStone");
                else
                    Engine::PlayAudio("Step");

            }
        }
    }
}
