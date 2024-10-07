#include "Player.h"
#include <iostream>

Player::Player()
{
    playerTexture = LoadTexture("Assets/Player.png");
    ragdollHeadTexture = LoadTexture("Assets/PlayerRagDollHead.png");
    ragdollBodyTexture = LoadTexture("Assets/PlayerRagDollBody.png");
    ragdollLegsTexture = LoadTexture("Assets/PlayerRagDollLegs.png");

    size = {(float) playerTexture.width, (float) playerTexture.height};
    initPhysicsBody();
    SetDrawPriority(LOW);
}

Player::~Player()
{
}

void Player::Update()
{
    if (!dead)
    {
        // Kill player on click in debug mode
        if (Engine::IsDebug())
        {
            if (IsMouseButtonPressed(0))
            {
                if (b2Shape_TestPoint(physShapeId, {Engine::GetMousePositionScaled().x, Engine::GetMousePositionScaled().y}))
                {
                    die();
                    return;
                }
            }
        }

        // Kill player if out of bounds
        if (position.x + size.x < 0 || position.x - size.x > Engine::GetInternalResolution().x || 
            position.y + size.y < 0 || position.y - size.y > Engine::GetInternalResolution().y)
        {
            die();
            return;
        }

        // Update collisions data
        int capacity = b2Body_GetContactCapacity(physBodyId);
        
        // Apply gravity
        if (capacity)
        {
            b2Vec2 finalVelocity = b2Body_GetLinearVelocity(physBodyId);
    
            if (IsKeyDown(KEY_LEFT) && finalVelocity.x > -speed)
                finalVelocity.x = -speed;
            
            if (IsKeyDown(KEY_RIGHT) && finalVelocity.x < speed)
                finalVelocity.x = speed;

            b2Body_SetLinearVelocity(physBodyId, finalVelocity);
        }
        

        // Get box2d result value
        b2Vec2 pos = b2Body_GetWorldPoint(physBodyId, { -extent.x, -extent.y });
        position = {pos.x, pos.y};
    }
}

void Player::Draw()
{    
    if (!dead)
    {
        DrawTextureEx(
            playerTexture,
            position,
            RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(physBodyId)), 
            1, 
            BLACK
        );
    }
    else
    {
        b2Vec2 headPos = b2Body_GetWorldPoint(ragdollHeadId, { -ragdollHeadExtent.x, -ragdollHeadExtent.y });
        b2Vec2 bodyPos = b2Body_GetWorldPoint(ragdollBodyId, { -ragdollBodyExtent.x, -ragdollBodyExtent.y });
        b2Vec2 legsPos = b2Body_GetWorldPoint(ragdollLegsId, { -ragdollLegsExtent.x, -ragdollLegsExtent.y });
        float headAngle = RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(ragdollHeadId));
        float bodyAngle = RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(ragdollBodyId));
        float legsAngle = RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(ragdollLegsId));

        DrawTextureEx(ragdollHeadTexture, {headPos.x, headPos.y}, headAngle, 1, WHITE);
        DrawTextureEx(ragdollBodyTexture, {bodyPos.x, bodyPos.y}, bodyAngle, 1, WHITE);
        DrawTextureEx(ragdollLegsTexture, {legsPos.x, legsPos.y}, legsAngle, 1, WHITE);
    }
}

void Player::SetPosition(Vector2 pos)
{
    position = pos;
    b2Body_SetTransform(physBodyId, {pos.x, pos.y}, b2Body_GetRotation(physBodyId));
}

void Player::initPhysicsBody()
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
}


void Player::die()
{
    dead = true;
    createRagdollBodies();
    b2DestroyBody(physBodyId);
}

void Player::createRagdollBodies()
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
    headShapeDef.friction = 0.05f;
    b2CreatePolygonShape(ragdollHeadId, &headShapeDef, &headBodyCube);
    drawPos.y += headSize.y;

    b2BodyDef bodyBodyDef = b2DefaultBodyDef();
    bodyBodyDef.type = b2_dynamicBody;
    bodyBodyDef.position = {drawPos.x, drawPos.y};
    ragdollBodyId = b2CreateBody(Engine::GetPhysWorldID(), &bodyBodyDef);
    b2Polygon bodyBodyCube = b2MakeBox(ragdollBodyExtent.x, ragdollBodyExtent.y);
    b2ShapeDef bodyShapeDef = b2DefaultShapeDef();
    bodyShapeDef.friction = 0.05f;
    b2CreatePolygonShape(ragdollBodyId, &bodyShapeDef, &bodyBodyCube);
    drawPos.y += bodySize.y;

    b2BodyDef legsBodyDef = b2DefaultBodyDef();
    legsBodyDef.type = b2_dynamicBody;
    legsBodyDef.position = {drawPos.x, drawPos.y};
    ragdollLegsId = b2CreateBody(Engine::GetPhysWorldID(), &legsBodyDef);
    b2Polygon legsBodyCube = b2MakeBox(ragdollLegsExtent.x, ragdollLegsExtent.y);
    b2ShapeDef legsShapeDef = b2DefaultShapeDef();
    legsShapeDef.friction = 0.05f;
    b2CreatePolygonShape(ragdollLegsId, &legsShapeDef, &legsBodyCube);

    // Setup joints
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

    // Apply same velocity to bodies as physBodyId
    b2Body_ApplyLinearImpulse(
        ragdollBodyId, 
        b2Body_GetLinearVelocity(physBodyId),
        b2Body_GetLocalCenterOfMass(ragdollBodyId), 
        true
    );

}
