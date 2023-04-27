#include "RSDK/Core/RetroEngine.hpp"

using namespace RSDK;

#if RETRO_REV0U
#include "Legacy/CollisionLegacy.cpp"
#endif

#if RETRO_REV0U
// Not sure why its 8.0 in v5U, it's 4.0 in v5 and v4, the "fix" is here since 8.0 causes issues with chibi due to his lil hitbox
#if RETRO_USE_ORIGINAL_CODE
#define COLLISION_OFFSET (TO_FIXED(8))
#else
#define COLLISION_OFFSET (TO_FIXED(4))
#endif
#else
#define COLLISION_OFFSET (TO_FIXED(4))
#endif

int32 RSDK::collisionTolerance = 0;
#if RETRO_REV0U
bool32 RSDK::useCollisionOffset = false;
#else
int32 RSDK::collisionOffset = 0;
#endif
int32 RSDK::collisionMaskAir = 0;

Hitbox RSDK::collisionOuter = { 0, 0, 0, 0 };
Hitbox RSDK::collisionInner = { 0, 0, 0, 0 };

Entity *RSDK::collisionEntity = NULL;

CollisionSensor RSDK::sensors[6];

#if RETRO_REV0U
#if RETRO_USE_ORIGINAL_CODE
// not sure why it's 24 here... it was 14 in all prev RSDK versions, maybe a mistake???
int32 RSDK::collisionMinimumDistance = TO_FIXED(24);
#else
int32 RSDK::collisionMinimumDistance = TO_FIXED(14);
#endif

uint8 RSDK::lowCollisionTolerance  = 8;
uint8 RSDK::highCollisionTolerance = 14;

uint8 RSDK::floorAngleTolerance = 0x20;
uint8 RSDK::wallAngleTolerance  = 0x20;
uint8 RSDK::roofAngleTolerance  = 0x20;
#else
#define collisionMinimumDistance (14)

#define lowCollisionTolerance  (8)
#define highCollisionTolerance (15)

#define floorAngleTolerance (0x20)
#define wallAngleTolerance  (0x20)
#define roofAngleTolerance  (0x20)
#endif

#if !RETRO_USE_ORIGINAL_CODE
bool32 RSDK::showHitboxes = false;

int32 RSDK::debugHitboxCount = 0;
DebugHitboxInfo RSDK::debugHitboxList[DEBUG_HITBOX_COUNT];

int32 RSDK::AddDebugHitbox(uint8 type, uint8 dir, Entity *entity, Hitbox *hitbox)
{
    int32 i = 0;
    for (; i < debugHitboxCount; ++i) {
        if (debugHitboxList[i].hitbox.left == hitbox->left && debugHitboxList[i].hitbox.top == hitbox->top
            && debugHitboxList[i].hitbox.right == hitbox->right && debugHitboxList[i].hitbox.bottom == hitbox->bottom
            && debugHitboxList[i].pos.x == entity->position.x && debugHitboxList[i].pos.y == entity->position.y
            && debugHitboxList[i].entity == entity) {
            return i;
        }
    }

    if (i < DEBUG_HITBOX_COUNT) {
        debugHitboxList[i].type          = type;
        debugHitboxList[i].entity        = entity;
        debugHitboxList[i].collision     = 0;
        debugHitboxList[i].hitbox.left   = hitbox->left;
        debugHitboxList[i].hitbox.top    = hitbox->top;
        debugHitboxList[i].hitbox.right  = hitbox->right;
        debugHitboxList[i].hitbox.bottom = hitbox->bottom;
        debugHitboxList[i].pos.x         = entity->position.x;
        debugHitboxList[i].pos.y         = entity->position.y;

        if ((dir & FLIP_X) == FLIP_X) {
            int32 store                     = -debugHitboxList[i].hitbox.left;
            debugHitboxList[i].hitbox.left  = -debugHitboxList[i].hitbox.right;
            debugHitboxList[i].hitbox.right = store;
        }
        if ((dir & FLIP_Y) == FLIP_Y) {
            int32 store                      = -debugHitboxList[i].hitbox.top;
            debugHitboxList[i].hitbox.top    = -debugHitboxList[i].hitbox.bottom;
            debugHitboxList[i].hitbox.bottom = store;
        }

        int32 id = debugHitboxCount;
        debugHitboxCount++;
        return id;
    }

    return -1;
}
#endif

#if RETRO_REV0U || RETRO_USE_MOD_LOADER
void RSDK::CopyCollisionMask(uint16 dst, uint16 src, uint8 cPlane, uint8 cMode)
{
    CollisionMask *srcMask = NULL;
    CollisionMask *dstMask = NULL;

    switch (cMode) {
        default: break;

        case CMODE_FLOOR:
            srcMask                                  = &collisionMasks[cPlane][src & 0x3FF];
            dstMask                                  = &collisionMasks[cPlane][dst & 0x3FF];
            tileInfo[cPlane][dst & 0x3FF].floorAngle = tileInfo[cPlane][src & 0x3FF].floorAngle;
            memcpy(dstMask->floorMasks, srcMask->floorMasks, sizeof(uint8) * TILE_SIZE);

            srcMask                                             = &collisionMasks[cPlane][(src & 0x3FF) * FLIP_X];
            dstMask                                             = &collisionMasks[cPlane][(dst & 0x3FF) * FLIP_X];
            tileInfo[cPlane][(dst & 0x3FF) * FLIP_X].floorAngle = tileInfo[cPlane][(src & 0x3FF) * FLIP_X].floorAngle;
            memcpy(dstMask->floorMasks, srcMask->floorMasks, sizeof(uint8) * TILE_SIZE);

            srcMask                                             = &collisionMasks[cPlane][(src & 0x3FF) * FLIP_Y];
            dstMask                                             = &collisionMasks[cPlane][(dst & 0x3FF) * FLIP_Y];
            tileInfo[cPlane][(dst & 0x3FF) * FLIP_Y].floorAngle = tileInfo[cPlane][(src & 0x3FF) * FLIP_Y].floorAngle;
            memcpy(dstMask->floorMasks, srcMask->floorMasks, sizeof(uint8) * TILE_SIZE);

            srcMask                                              = &collisionMasks[cPlane][(src & 0x3FF) * FLIP_XY];
            dstMask                                              = &collisionMasks[cPlane][(dst & 0x3FF) * FLIP_XY];
            tileInfo[cPlane][(dst & 0x3FF) * FLIP_XY].floorAngle = tileInfo[cPlane][(src & 0x3FF) * FLIP_XY].floorAngle;
            memcpy(dstMask->floorMasks, srcMask->floorMasks, sizeof(uint8) * TILE_SIZE);
            break;

        case CMODE_LWALL:
            srcMask                                  = &collisionMasks[cPlane][src & 0x3FF];
            dstMask                                  = &collisionMasks[cPlane][dst & 0x3FF];
            tileInfo[cPlane][dst & 0x3FF].lWallAngle = tileInfo[cPlane][src & 0x3FF].lWallAngle;
            memcpy(dstMask->lWallMasks, srcMask->lWallMasks, sizeof(uint8) * TILE_SIZE);

            srcMask                                             = &collisionMasks[cPlane][(src & 0x3FF) * FLIP_X];
            dstMask                                             = &collisionMasks[cPlane][(dst & 0x3FF) * FLIP_X];
            tileInfo[cPlane][(dst & 0x3FF) * FLIP_X].lWallAngle = tileInfo[cPlane][(src & 0x3FF) * FLIP_X].lWallAngle;
            memcpy(dstMask->lWallMasks, srcMask->lWallMasks, sizeof(uint8) * TILE_SIZE);

            srcMask                                             = &collisionMasks[cPlane][(src & 0x3FF) * FLIP_Y];
            dstMask                                             = &collisionMasks[cPlane][(dst & 0x3FF) * FLIP_Y];
            tileInfo[cPlane][(dst & 0x3FF) * FLIP_Y].lWallAngle = tileInfo[cPlane][(src & 0x3FF) * FLIP_Y].lWallAngle;
            memcpy(dstMask->lWallMasks, srcMask->lWallMasks, sizeof(uint8) * TILE_SIZE);

            srcMask                                              = &collisionMasks[cPlane][(src & 0x3FF) * FLIP_XY];
            dstMask                                              = &collisionMasks[cPlane][(dst & 0x3FF) * FLIP_XY];
            tileInfo[cPlane][(dst & 0x3FF) * FLIP_XY].lWallAngle = tileInfo[cPlane][(src & 0x3FF) * FLIP_XY].lWallAngle;
            memcpy(dstMask->lWallMasks, srcMask->lWallMasks, sizeof(uint8) * TILE_SIZE);
            break;

        case CMODE_ROOF:
            srcMask                                 = &collisionMasks[cPlane][src & 0x3FF];
            dstMask                                 = &collisionMasks[cPlane][dst & 0x3FF];
            tileInfo[cPlane][dst & 0x3FF].roofAngle = tileInfo[cPlane][src & 0x3FF].roofAngle;
            memcpy(dstMask->roofMasks, srcMask->roofMasks, sizeof(uint8) * TILE_SIZE);

            srcMask                                            = &collisionMasks[cPlane][(src & 0x3FF) * FLIP_X];
            dstMask                                            = &collisionMasks[cPlane][(dst & 0x3FF) * FLIP_X];
            tileInfo[cPlane][(dst & 0x3FF) * FLIP_X].roofAngle = tileInfo[cPlane][(src & 0x3FF) * FLIP_X].roofAngle;
            memcpy(dstMask->roofMasks, srcMask->roofMasks, sizeof(uint8) * TILE_SIZE);

            srcMask                                            = &collisionMasks[cPlane][(src & 0x3FF) * FLIP_Y];
            dstMask                                            = &collisionMasks[cPlane][(dst & 0x3FF) * FLIP_Y];
            tileInfo[cPlane][(dst & 0x3FF) * FLIP_Y].roofAngle = tileInfo[cPlane][(src & 0x3FF) * FLIP_Y].roofAngle;
            memcpy(dstMask->roofMasks, srcMask->roofMasks, sizeof(uint8) * TILE_SIZE);

            srcMask                                             = &collisionMasks[cPlane][(src & 0x3FF) * FLIP_XY];
            dstMask                                             = &collisionMasks[cPlane][(dst & 0x3FF) * FLIP_XY];
            tileInfo[cPlane][(dst & 0x3FF) * FLIP_XY].roofAngle = tileInfo[cPlane][(src & 0x3FF) * FLIP_XY].roofAngle;
            memcpy(dstMask->roofMasks, srcMask->roofMasks, sizeof(uint8) * TILE_SIZE);
            break;

        case CMODE_RWALL:
            srcMask                                  = &collisionMasks[cPlane][src & 0x3FF];
            dstMask                                  = &collisionMasks[cPlane][dst & 0x3FF];
            tileInfo[cPlane][dst & 0x3FF].rWallAngle = tileInfo[cPlane][src & 0x3FF].rWallAngle;
            memcpy(dstMask->rWallMasks, srcMask->rWallMasks, sizeof(uint8) * TILE_SIZE);

            srcMask                                             = &collisionMasks[cPlane][(src & 0x3FF) * FLIP_X];
            dstMask                                             = &collisionMasks[cPlane][(dst & 0x3FF) * FLIP_X];
            tileInfo[cPlane][(dst & 0x3FF) * FLIP_X].rWallAngle = tileInfo[cPlane][(src & 0x3FF) * FLIP_X].rWallAngle;
            memcpy(dstMask->rWallMasks, srcMask->rWallMasks, sizeof(uint8) * TILE_SIZE);

            srcMask                                             = &collisionMasks[cPlane][(src & 0x3FF) * FLIP_Y];
            dstMask                                             = &collisionMasks[cPlane][(dst & 0x3FF) * FLIP_Y];
            tileInfo[cPlane][(dst & 0x3FF) * FLIP_Y].rWallAngle = tileInfo[cPlane][(src & 0x3FF) * FLIP_Y].rWallAngle;
            memcpy(dstMask->rWallMasks, srcMask->rWallMasks, sizeof(uint8) * TILE_SIZE);

            srcMask                                              = &collisionMasks[cPlane][(src & 0x3FF) * FLIP_XY];
            dstMask                                              = &collisionMasks[cPlane][(dst & 0x3FF) * FLIP_XY];
            tileInfo[cPlane][(dst & 0x3FF) * FLIP_XY].rWallAngle = tileInfo[cPlane][(src & 0x3FF) * FLIP_XY].rWallAngle;
            memcpy(dstMask->rWallMasks, srcMask->rWallMasks, sizeof(uint8) * TILE_SIZE);
            break;
    }
}
#endif

bool32 RSDK::CheckObjectCollisionTouch(Entity *thisEntity, Hitbox *thisHitbox, Entity *otherEntity, Hitbox *otherHitbox)
{
    int32 store = 0;
    if (!thisEntity || !otherEntity || !thisHitbox || !otherHitbox)
        return false;

    if ((thisEntity->direction & FLIP_X) == FLIP_X) {
        store             = -thisHitbox->left;
        thisHitbox->left  = -thisHitbox->right;
        thisHitbox->right = store;

        store              = -otherHitbox->left;
        otherHitbox->left  = -otherHitbox->right;
        otherHitbox->right = store;
    }
    if ((thisEntity->direction & FLIP_Y) == FLIP_Y) {
        store              = -thisHitbox->top;
        thisHitbox->top    = -thisHitbox->bottom;
        thisHitbox->bottom = store;

        store               = -otherHitbox->top;
        otherHitbox->top    = -otherHitbox->bottom;
        otherHitbox->bottom = store;
    }

    int32 thisIX  = FROM_FIXED(thisEntity->position.x);
    int32 thisIY  = FROM_FIXED(thisEntity->position.y);
    int32 otherIX = FROM_FIXED(otherEntity->position.x);
    int32 otherIY = FROM_FIXED(otherEntity->position.y);

    bool32 collided = thisIX + thisHitbox->left < otherIX + otherHitbox->right && thisIX + thisHitbox->right > otherIX + otherHitbox->left
                      && thisIY + thisHitbox->top < otherIY + otherHitbox->bottom && thisIY + thisHitbox->bottom > otherIY + otherHitbox->top;

    if ((thisEntity->direction & FLIP_X) == FLIP_X) {
        store             = -thisHitbox->left;
        thisHitbox->left  = -thisHitbox->right;
        thisHitbox->right = store;

        store              = -otherHitbox->left;
        otherHitbox->left  = -otherHitbox->right;
        otherHitbox->right = store;
    }
    if ((thisEntity->direction & FLIP_Y) == FLIP_Y) {
        store              = -thisHitbox->top;
        thisHitbox->top    = -thisHitbox->bottom;
        thisHitbox->bottom = store;

        store               = -otherHitbox->top;
        otherHitbox->top    = -otherHitbox->bottom;
        otherHitbox->bottom = store;
    }

#if !RETRO_USE_ORIGINAL_CODE
    if (showHitboxes) {
        int32 thisHitboxID  = RSDK::AddDebugHitbox(H_TYPE_TOUCH, thisEntity->direction, thisEntity, thisHitbox);
        int32 otherHitboxID = RSDK::AddDebugHitbox(H_TYPE_TOUCH, thisEntity->direction, otherEntity, otherHitbox);

        if (thisHitboxID >= 0 && collided)
            debugHitboxList[thisHitboxID].collision |= 1 << (collided - 1);
        if (otherHitboxID >= 0 && collided)
            debugHitboxList[otherHitboxID].collision |= 1 << (collided - 1);
    }
#endif

    return collided;
}

uint8 RSDK::CheckObjectCollisionBox(Entity *thisEntity, Hitbox *thisHitbox, Entity *otherEntity, Hitbox *otherHitbox, bool32 setValues)
{
    if (!thisEntity || !otherEntity || !thisHitbox || !otherHitbox)
        return C_NONE;

    int32 collisionSideH = C_NONE;
    int32 collisionSideV = C_NONE;

    int32 collideX = otherEntity->position.x;
    int32 collideY = otherEntity->position.y;

    if ((thisEntity->direction & FLIP_X) == FLIP_X) {
        int32 store       = -thisHitbox->left;
        thisHitbox->left  = -thisHitbox->right;
        thisHitbox->right = store;

        store              = -otherHitbox->left;
        otherHitbox->left  = -otherHitbox->right;
        otherHitbox->right = store;
    }

    if ((thisEntity->direction & FLIP_Y) == FLIP_Y) {
        int32 store        = -thisHitbox->top;
        thisHitbox->top    = -thisHitbox->bottom;
        thisHitbox->bottom = store;

        store               = -otherHitbox->top;
        otherHitbox->top    = -otherHitbox->bottom;
        otherHitbox->bottom = store;
    }

    int32 thisIX  = FROM_FIXED(thisEntity->position.x);
    int32 thisIY  = FROM_FIXED(thisEntity->position.y);
    int32 otherIX = FROM_FIXED(otherEntity->position.x);
    int32 otherIY = FROM_FIXED(otherEntity->position.y);

    otherHitbox->top++;
    otherHitbox->bottom--;

    if (otherIX <= (thisHitbox->right + thisHitbox->left + 2 * thisIX) >> 1) {
        if (otherIX + otherHitbox->right >= thisIX + thisHitbox->left && thisIY + thisHitbox->top < otherIY + otherHitbox->bottom
            && thisIY + thisHitbox->bottom > otherIY + otherHitbox->top) {
            collisionSideH = C_LEFT;
            collideX       = thisEntity->position.x + TO_FIXED(thisHitbox->left - otherHitbox->right);
        }
    }
    else {
        if (otherIX + otherHitbox->left < thisIX + thisHitbox->right && thisIY + thisHitbox->top < otherIY + otherHitbox->bottom
            && thisIY + thisHitbox->bottom > otherIY + otherHitbox->top) {
            collisionSideH = C_RIGHT;
            collideX       = thisEntity->position.x + TO_FIXED(thisHitbox->right - otherHitbox->left);
        }
    }

    otherHitbox->left++;
    otherHitbox->top--;
    otherHitbox->right--;
    otherHitbox->bottom++;

    if (otherIY <= thisIY + ((thisHitbox->top + thisHitbox->bottom) >> 1)) {
        if (otherIY + otherHitbox->bottom >= thisIY + thisHitbox->top && thisIX + thisHitbox->left < otherIX + otherHitbox->right
            && thisIX + thisHitbox->right > otherIX + otherHitbox->left) {
            collisionSideV = C_TOP;
            collideY       = thisEntity->position.y + TO_FIXED(thisHitbox->top - otherHitbox->bottom);
        }
    }
    else {
        if (otherIY + otherHitbox->top < thisIY + thisHitbox->bottom && thisIX + thisHitbox->left < otherIX + otherHitbox->right) {
            if (otherIX + otherHitbox->left < thisIX + thisHitbox->right) {
                collisionSideV = C_BOTTOM;
                collideY       = thisEntity->position.y + TO_FIXED(thisHitbox->bottom - otherHitbox->top);
            }
        }
    }

    otherHitbox->left--;
    otherHitbox->right++;

    if ((thisEntity->direction & FLIP_X) == FLIP_X) {
        int32 store       = -thisHitbox->left;
        thisHitbox->left  = -thisHitbox->right;
        thisHitbox->right = store;

        store              = -otherHitbox->left;
        otherHitbox->left  = -otherHitbox->right;
        otherHitbox->right = store;
    }

    if ((thisEntity->direction & FLIP_Y) == FLIP_Y) {
        int32 store        = -thisHitbox->top;
        thisHitbox->top    = -thisHitbox->bottom;
        thisHitbox->bottom = store;

        store               = -otherHitbox->top;
        otherHitbox->top    = -otherHitbox->bottom;
        otherHitbox->bottom = store;
    }

    uint8 side = C_NONE;

    int32 cx = FROM_FIXED(collideX - otherEntity->position.x);
    int32 cy = FROM_FIXED(collideY - otherEntity->position.y);
    if ((cx * cx >= cy * cy && (collisionSideV || !collisionSideH)) || (!collisionSideH && collisionSideV)) {
        side = collisionSideV;
    }
    else {
        side = collisionSideH;
    }

    if (setValues) {
        int32 velX = 0;
        switch (side) {
            default:
            case C_NONE: break;

            case C_TOP:
                otherEntity->position.y = collideY;

                if (otherEntity->velocity.y > 0)
                    otherEntity->velocity.y = 0;

#if RETRO_REV0U
                if (otherEntity->tileCollisions != TILECOLLISION_UP) {
#endif
                    if (!otherEntity->onGround && otherEntity->velocity.y >= 0) {
                        otherEntity->groundVel = otherEntity->velocity.x;
                        otherEntity->angle     = 0x00;
                        otherEntity->onGround  = true;
                    }
#if RETRO_REV0U
                }
#endif
                break;

            case C_LEFT:
                otherEntity->position.x = collideX;

                velX = otherEntity->velocity.x;
                if (otherEntity->onGround) {
                    if (otherEntity->collisionMode == CMODE_ROOF)
                        velX = -otherEntity->groundVel;
                    else
                        velX = otherEntity->groundVel;
                }

                if (velX > 0) {
                    otherEntity->velocity.x = 0;
                    otherEntity->groundVel  = 0;
                }
                break;

            case C_RIGHT:
                otherEntity->position.x = collideX;

                velX = otherEntity->velocity.x;
                if (otherEntity->onGround) {
                    if (otherEntity->collisionMode == CMODE_ROOF)
                        velX = -otherEntity->groundVel;
                    else
                        velX = otherEntity->groundVel;
                }

                if (velX < 0) {
                    otherEntity->velocity.x = 0;
                    otherEntity->groundVel  = 0;
                }
                break;

            case C_BOTTOM:
                otherEntity->position.y = collideY;

                if (otherEntity->velocity.y < 0)
                    otherEntity->velocity.y = 0;

#if RETRO_REV0U
                if (otherEntity->tileCollisions == TILECOLLISION_UP) {
                    if (!otherEntity->onGround && otherEntity->velocity.y <= 0) {
                        otherEntity->angle     = 0x80;
                        otherEntity->groundVel = -otherEntity->velocity.x;
                        otherEntity->onGround  = true;
                    }
                }
#endif
                break;
        }
    }
    else {
#if !RETRO_REV0U
        switch (side) {
            default:
            case C_NONE: break;
            case C_TOP: otherEntity->position.y = collideY; break;
            case C_LEFT: otherEntity->position.x = collideX; break;
            case C_RIGHT: otherEntity->position.x = collideX; break;
            case C_BOTTOM: otherEntity->position.y = collideY; break;
        }
#endif
    }

#if !RETRO_USE_ORIGINAL_CODE
    if (showHitboxes) {
        int32 thisHitboxID  = RSDK::AddDebugHitbox(H_TYPE_BOX, thisEntity->direction, thisEntity, thisHitbox);
        int32 otherHitboxID = RSDK::AddDebugHitbox(H_TYPE_BOX, thisEntity->direction, otherEntity, otherHitbox);

        if (thisHitboxID >= 0 && side)
            debugHitboxList[thisHitboxID].collision |= 1 << (side - 1);
        if (otherHitboxID >= 0 && side)
            debugHitboxList[otherHitboxID].collision |= 1 << (4 - side);
    }
#endif
    return side;
}

bool32 RSDK::CheckObjectCollisionPlatform(Entity *thisEntity, Hitbox *thisHitbox, Entity *otherEntity, Hitbox *otherHitbox, bool32 setValues)
{
    int32 store     = 0;
    bool32 collided = false;

    if (!thisEntity || !otherEntity || !thisHitbox || !otherHitbox)
        return false;

    if ((thisEntity->direction & FLIP_X) == FLIP_X) {
        store             = -thisHitbox->left;
        thisHitbox->left  = -thisHitbox->right;
        thisHitbox->right = store;

        store              = -otherHitbox->left;
        otherHitbox->left  = -otherHitbox->right;
        otherHitbox->right = store;
    }
    if ((thisEntity->direction & FLIP_Y) == FLIP_Y) {
        store              = -thisHitbox->top;
        thisHitbox->top    = -thisHitbox->bottom;
        thisHitbox->bottom = store;

        store               = -otherHitbox->top;
        otherHitbox->top    = -otherHitbox->bottom;
        otherHitbox->bottom = store;
    }

    int32 thisIX  = FROM_FIXED(thisEntity->position.x);
    int32 thisIY  = FROM_FIXED(thisEntity->position.y);
    int32 otherIX = FROM_FIXED(otherEntity->position.x);
    int32 otherIY = FROM_FIXED(otherEntity->position.y);

    int32 otherMoveY = FROM_FIXED(otherEntity->position.y - otherEntity->velocity.y);

#if RETRO_REV0U
    if (otherEntity->tileCollisions == TILECOLLISION_UP) {
        if (otherIY - otherHitbox->bottom >= thisIY + thisHitbox->top && otherMoveY - otherHitbox->bottom <= thisIY + thisHitbox->bottom
            && thisIX + thisHitbox->left < otherIX + otherHitbox->right && thisIX + thisHitbox->right > otherIX + otherHitbox->left
            && otherEntity->velocity.y <= 0) {

            otherEntity->position.y = thisEntity->position.y + TO_FIXED(thisHitbox->bottom + otherHitbox->bottom);

            if (setValues) {
                otherEntity->velocity.y = 0;

                if (!otherEntity->onGround) {
                    otherEntity->groundVel = -otherEntity->velocity.x;
                    otherEntity->angle     = 0x80;
                    otherEntity->onGround  = true;
                }
            }

            collided = true;
        }
    }
    else {
#endif
        if (otherIY + otherHitbox->bottom >= thisIY + thisHitbox->top && otherMoveY + otherHitbox->bottom <= thisIY + thisHitbox->bottom
            && thisIX + thisHitbox->left < otherIX + otherHitbox->right && thisIX + thisHitbox->right > otherIX + otherHitbox->left
            && otherEntity->velocity.y >= 0) {

            otherEntity->position.y = thisEntity->position.y + TO_FIXED(thisHitbox->top - otherHitbox->bottom);

            if (setValues) {
                otherEntity->velocity.y = 0;

                if (!otherEntity->onGround) {
                    otherEntity->groundVel = otherEntity->velocity.x;
                    otherEntity->angle     = 0x00;
                    otherEntity->onGround  = true;
                }
            }

            collided = true;
        }
#if RETRO_REV0U
    }
#endif

    if ((thisEntity->direction & FLIP_X) == FLIP_X) {
        store             = -thisHitbox->left;
        thisHitbox->left  = -thisHitbox->right;
        thisHitbox->right = store;

        store              = -otherHitbox->left;
        otherHitbox->left  = -otherHitbox->right;
        otherHitbox->right = store;
    }

    if ((thisEntity->direction & FLIP_Y) == FLIP_Y) {
        store              = -thisHitbox->top;
        thisHitbox->top    = -thisHitbox->bottom;
        thisHitbox->bottom = store;

        store               = -otherHitbox->top;
        otherHitbox->top    = -otherHitbox->bottom;
        otherHitbox->bottom = store;
    }

#if !RETRO_USE_ORIGINAL_CODE
    if (showHitboxes) {
        int32 thisHitboxID  = RSDK::AddDebugHitbox(H_TYPE_PLAT, thisEntity->direction, thisEntity, thisHitbox);
        int32 otherHitboxID = RSDK::AddDebugHitbox(H_TYPE_PLAT, thisEntity->direction, otherEntity, otherHitbox);
#if RETRO_REV0U
        if (otherEntity->tileCollisions == TILECOLLISION_UP) {

            if (thisHitboxID >= 0 && collided)
                debugHitboxList[thisHitboxID].collision |= 1 << 3;
            if (otherHitboxID >= 0 && collided)
                debugHitboxList[otherHitboxID].collision |= 1 << 0;
        }
        else {
#endif
            if (thisHitboxID >= 0 && collided)
                debugHitboxList[thisHitboxID].collision |= 1 << 0;
            if (otherHitboxID >= 0 && collided)
                debugHitboxList[otherHitboxID].collision |= 1 << 3;
#if RETRO_REV0U
        }
#endif
    }
#endif

    return collided;
}

bool32 RSDK::ObjectTileCollision(Entity *entity, uint16 cLayers, uint8 cMode, uint8 cPlane, int32 xOffset, int32 yOffset, bool32 setPos)
{
    int32 layerID   = 1;
    bool32 collided = false;
    int32 posX      = FROM_FIXED(xOffset + entity->position.x);
    int32 posY      = FROM_FIXED(yOffset + entity->position.y);

    int32 solid = 0;
    switch (cMode) {
        default: return false;

        case CMODE_FLOOR:
            solid = cPlane ? (1 << 14) : (1 << 12);

            for (int32 l = 0; l < LAYER_COUNT; ++l, layerID <<= 1) {
                if (cLayers & layerID) {
                    TileLayer *layer = &tileLayers[l];
                    int32 colX       = posX - layer->position.x;
                    int32 colY       = posY - layer->position.y;
                    int32 cy         = (colY & -TILE_SIZE) - TILE_SIZE;
                    if (colX >= 0 && colX < TILE_SIZE * layer->xsize) {
                        for (int32 i = 0; i < 3; ++i) {
                            if (cy >= 0 && cy < TILE_SIZE * layer->ysize) {
                                uint16 tile = layer->layout[(colX / TILE_SIZE) + ((cy / TILE_SIZE) << layer->widthShift)];
                                if (tile < 0xFFFF && tile & solid) {
                                    int32 ty = cy + collisionMasks[cPlane][tile & 0xFFF].floorMasks[colX & 0xF];
                                    if (colY >= ty && abs(colY - ty) <= 14) {
                                        collided = true;
                                        colY     = ty;
                                        i        = 3;
                                    }
                                }
                            }
                            cy += TILE_SIZE;
                        }
                    }
                    posX = layer->position.x + colX;
                    posY = layer->position.y + colY;
                }
            }

            if (setPos && collided)
                entity->position.y = TO_FIXED(posY) - yOffset;
            return collided;

        case CMODE_LWALL:
            solid = cPlane ? (1 << 15) : (1 << 13);

            for (int32 l = 0; l < LAYER_COUNT; ++l, layerID <<= 1) {
                if (cLayers & layerID) {
                    TileLayer *layer = &tileLayers[l];
                    int32 colX       = posX - layer->position.x;
                    int32 colY       = posY - layer->position.y;
                    int32 cx         = (colX & -TILE_SIZE) - TILE_SIZE;
                    if (colY >= 0 && colY < TILE_SIZE * layer->ysize) {
                        for (int32 i = 0; i < 3; ++i) {
                            if (cx >= 0 && cx < TILE_SIZE * layer->xsize) {
                                uint16 tile = layer->layout[(cx >> 4) + ((colY / TILE_SIZE) << layer->widthShift)];
                                if (tile < 0xFFFF && tile & solid) {
                                    int32 tx = cx + collisionMasks[cPlane][tile & 0xFFF].lWallMasks[colY & 0xF];
                                    if (colX >= tx && abs(colX - tx) <= 14) {
                                        collided = true;
                                        colX     = tx;
                                        i        = 3;
                                    }
                                }
                            }
                            cx += TILE_SIZE;
                        }
                    }
                    posX = layer->position.x + colX;
                    posY = layer->position.y + colY;
                }
            }

            if (setPos && collided)
                entity->position.x = TO_FIXED(posX) - xOffset;
            return collided;

        case CMODE_ROOF:
            solid = cPlane ? (1 << 15) : (1 << 13);

            for (int32 l = 0; l < LAYER_COUNT; ++l, layerID <<= 1) {
                if (cLayers & layerID) {
                    TileLayer *layer = &tileLayers[l];
                    int32 colX       = posX - layer->position.x;
                    int32 colY       = posY - layer->position.y;
                    int32 cy         = (colY & -TILE_SIZE) + TILE_SIZE;
                    if (colX >= 0 && colX < TILE_SIZE * layer->xsize) {
                        for (int32 i = 0; i < 3; ++i) {
                            if (cy >= 0 && cy < TILE_SIZE * layer->ysize) {
                                uint16 tile = layer->layout[(colX >> 4) + ((cy / TILE_SIZE) << layer->widthShift)];
                                if (tile < 0xFFFF && tile & solid) {
                                    int32 ty = cy + collisionMasks[cPlane][tile & 0xFFF].roofMasks[colX & 0xF];
                                    if (colY <= ty && abs(colY - ty) <= 14) {
                                        collided = true;
                                        colY     = ty;
                                        i        = 3;
                                    }
                                }
                            }
                            cy -= TILE_SIZE;
                        }
                    }
                    posX = layer->position.x + colX;
                    posY = layer->position.y + colY;
                }
            }

            if (setPos && collided)
                entity->position.y = TO_FIXED(posY) - yOffset;
            return collided;

        case CMODE_RWALL:
            solid = cPlane ? (1 << 15) : (1 << 13);

            for (int32 l = 0; l < LAYER_COUNT; ++l, layerID <<= 1) {
                if (cLayers & layerID) {
                    TileLayer *layer = &tileLayers[l];
                    int32 colX       = posX - layer->position.x;
                    int32 colY       = posY - layer->position.y;
                    int32 cx         = (colX & -TILE_SIZE) + TILE_SIZE;
                    if (colY >= 0 && colY < TILE_SIZE * layer->ysize) {
                        for (int32 i = 0; i < 3; ++i) {
                            if (cx >= 0 && cx < TILE_SIZE * layer->xsize) {
                                uint16 tile = layer->layout[(cx >> 4) + ((colY / TILE_SIZE) << layer->widthShift)];
                                if (tile < 0xFFFF && tile & solid) {
                                    int32 tx = cx + collisionMasks[cPlane][tile & 0xFFF].rWallMasks[colY & 0xF];
                                    if (colX <= tx && abs(colX - tx) <= 14) {
                                        collided = true;
                                        colX     = tx;
                                        i        = 3;
                                    }
                                }
                            }
                            cx -= TILE_SIZE;
                        }
                    }
                    posX = layer->position.x + colX;
                    posY = layer->position.y + colY;
                }
            }

            if (setPos && collided)
                entity->position.x = TO_FIXED(posX) - xOffset;
            return collided;
    }
}
bool32 RSDK::ObjectTileGrip(Entity *entity, uint16 cLayers, uint8 cMode, uint8 cPlane, int32 xOffset, int32 yOffset, int32 tolerance)
{
    int32 layerID   = 1;
    bool32 collided = false;
    int32 posX      = FROM_FIXED(xOffset + entity->position.x);
    int32 posY      = FROM_FIXED(yOffset + entity->position.y);

    int32 solid = 0;
    switch (cMode) {
        default: return false;

        case CMODE_FLOOR:
            solid = cPlane ? (1 << 14) : (1 << 12);

            for (int32 l = 0; l < LAYER_COUNT; ++l, layerID <<= 1) {
                if (cLayers & layerID) {
                    TileLayer *layer = &tileLayers[l];
                    int32 colX       = posX - layer->position.x;
                    int32 colY       = posY - layer->position.y;
                    int32 cy         = (colY & -TILE_SIZE) - TILE_SIZE;
                    if (colX >= 0 && colX < TILE_SIZE * layer->xsize) {
                        for (int32 i = 0; i < 3; ++i) {
                            if (cy >= 0 && cy < TILE_SIZE * layer->ysize) {
                                uint16 tile = layer->layout[(colX >> 4) + ((cy / TILE_SIZE) << layer->widthShift)];
                                if (tile < 0xFFFF && tile & solid) {
                                    int32 mask = collisionMasks[cPlane][tile & 0xFFF].floorMasks[colX & 0xF];
                                    int32 ty   = cy + mask;
                                    if (mask < 0xFF) {
                                        if (abs(colY - ty) <= tolerance) {
                                            collided = true;
                                            colY     = ty;
                                        }
                                        i = 3;
                                    }
                                }
                            }
                            cy += TILE_SIZE;
                        }
                    }
                    posX = layer->position.x + colX;
                    posY = layer->position.y + colY;
                }
            }

            if (collided)
                entity->position.y = TO_FIXED(posY) - yOffset;
            return collided;

        case CMODE_LWALL:
            solid = cPlane ? (1 << 15) : (1 << 13);

            for (int32 l = 0; l < LAYER_COUNT; ++l, layerID <<= 1) {
                if (cLayers & layerID) {
                    TileLayer *layer = &tileLayers[l];
                    int32 colX       = posX - layer->position.x;
                    int32 colY       = posY - layer->position.y;
                    int32 cx         = (colX & -TILE_SIZE) - TILE_SIZE;
                    if (colY >= 0 && colY < TILE_SIZE * layer->ysize) {
                        for (int32 i = 0; i < 3; ++i) {
                            if (cx >= 0 && cx < TILE_SIZE * layer->xsize) {
                                uint16 tile = layer->layout[(cx >> 4) + ((colY / TILE_SIZE) << layer->widthShift)];
                                if (tile < 0xFFFF && tile & solid) {
                                    int32 mask = collisionMasks[cPlane][tile & 0xFFF].lWallMasks[colY & 0xF];
                                    int32 tx   = cx + mask;
                                    if (mask < 0xFF) {
                                        if (abs(colX - tx) <= tolerance) {
                                            collided = true;
                                            colX     = tx;
                                        }
                                        i = 3;
                                    }
                                }
                            }
                            cx += TILE_SIZE;
                        }
                    }
                    posX = layer->position.x + colX;
                    posY = layer->position.y + colY;
                }
            }

            if (collided)
                entity->position.x = TO_FIXED(posX) - xOffset;
            return collided;

        case CMODE_ROOF:
            solid = cPlane ? (1 << 15) : (1 << 13);

            for (int32 l = 0; l < LAYER_COUNT; ++l, layerID <<= 1) {
                if (cLayers & layerID) {
                    TileLayer *layer = &tileLayers[l];
                    int32 colX       = posX - layer->position.x;
                    int32 colY       = posY - layer->position.y;
                    int32 cy         = (colY & -TILE_SIZE) + TILE_SIZE;
                    if (colX >= 0 && colX < TILE_SIZE * layer->xsize) {
                        for (int32 i = 0; i < 3; ++i) {
                            if (cy >= 0 && cy < TILE_SIZE * layer->ysize) {
                                uint16 tile = layer->layout[(colX >> 4) + ((cy / TILE_SIZE) << layer->widthShift)];
                                if (tile < 0xFFFF && tile & solid) {
                                    int32 mask = collisionMasks[cPlane][tile & 0xFFF].roofMasks[colX & 0xF];
                                    int32 ty   = cy + mask;
                                    if (mask < 0xFF) {
                                        if (abs(colY - ty) <= tolerance) {
                                            collided = true;
                                            colY     = ty;
                                        }
                                        i = 3;
                                    }
                                }
                            }
                            cy -= TILE_SIZE;
                        }
                    }
                    posX = layer->position.x + colX;
                    posY = layer->position.y + colY;
                }
            }

            if (collided)
                entity->position.y = TO_FIXED(posY) - yOffset;
            return collided;

        case CMODE_RWALL:
            solid = cPlane ? (1 << 15) : (1 << 13);

            for (int32 l = 0; l < LAYER_COUNT; ++l, layerID <<= 1) {
                if (cLayers & layerID) {
                    TileLayer *layer = &tileLayers[l];
                    int32 colX       = posX - layer->position.x;
                    int32 colY       = posY - layer->position.y;
                    int32 cx         = (colX & -TILE_SIZE) + TILE_SIZE;
                    if (colY >= 0 && colY < TILE_SIZE * layer->ysize) {
                        for (int32 i = 0; i < 3; ++i) {
                            if (cx >= 0 && cx < TILE_SIZE * layer->xsize) {
                                uint16 tile = layer->layout[(cx >> 4) + ((colY / TILE_SIZE) << layer->widthShift)];
                                if (tile < 0xFFFF && tile & solid) {
                                    int32 mask = collisionMasks[cPlane][tile & 0xFFF].rWallMasks[colY & 0xF];
                                    int32 tx   = cx + mask;
                                    if (mask < 0xFF) {
                                        if (abs(colX - tx) <= tolerance) {
                                            collided = true;
                                            colX     = tx;
                                        }
                                        i = 3;
                                    }
                                }
                            }
                            cx -= TILE_SIZE;
                        }
                    }
                    posX = layer->position.x + colX;
                    posY = layer->position.y + colY;
                }
            }

            if (collided)
                entity->position.x = TO_FIXED(posX) - xOffset;
            return collided;
    }
}

void RSDK::ProcessObjectMovement(Entity *entity, Hitbox *outerBox, Hitbox *innerBox)
{
    if (entity && outerBox && innerBox) {
        if (entity->tileCollisions) {
            entity->angle &= 0xFF;

            collisionTolerance = highCollisionTolerance;
            if (abs(entity->groundVel) < TO_FIXED(6) && entity->angle == 0)
                collisionTolerance = lowCollisionTolerance;

            collisionOuter.left   = outerBox->left;
            collisionOuter.top    = outerBox->top;
            collisionOuter.right  = outerBox->right;
            collisionOuter.bottom = outerBox->bottom;

            collisionInner.left   = innerBox->left;
            collisionInner.top    = innerBox->top;
            collisionInner.right  = innerBox->right;
            collisionInner.bottom = innerBox->bottom;

            collisionEntity = entity;

#if RETRO_REV0U
            collisionMaskAir = collisionOuter.bottom >= 14 ? 19 : 17;

            if (entity->onGround) {
                // true = normal, false = flipped
                if (entity->tileCollisions == TILECOLLISION_DOWN)
                    useCollisionOffset = entity->angle == 0x00;
                else
                    useCollisionOffset = entity->angle == 0x80;

#if !RETRO_USE_ORIGINAL_CODE
                // fixes some clipping issues as chibi sonic (& using small hitboxes)
                // shouldn't effect anything else :)
                if (collisionOuter.bottom < 14)
                    useCollisionOffset = false;
#endif

                ProcessPathGrip();
            }
            else {
                useCollisionOffset = false;
                // true = normal, false = flipped
                if (entity->tileCollisions == TILECOLLISION_DOWN)
                    ProcessAirCollision_Down();
                else
                    ProcessAirCollision_Up();
            }
#else
            if (collisionOuter.bottom >= 14) {
                collisionOffset  = COLLISION_OFFSET;
                collisionMaskAir = 19;
            }
            else {
                collisionOffset    = 0;
                collisionTolerance = 15;
                collisionMaskAir   = 17;
            }

            if (entity->onGround)
                ProcessPathGrip();
            else
                ProcessAirCollision_Down();
#endif

            if (entity->onGround) {
                entity->velocity.x = entity->groundVel * cos256LookupTable[entity->angle & 0xFF] >> 8;
                entity->velocity.y = entity->groundVel * sin256LookupTable[entity->angle & 0xFF] >> 8;
            }
            else {
                entity->groundVel = entity->velocity.x;
            }
        }
        else {
            entity->position.x += entity->velocity.x;
            entity->position.y += entity->velocity.y;
        }
    }
}

void RSDK::ProcessAirCollision_Down()
{
    uint8 movingDown  = 0;
    uint8 movingUp    = 0;
    uint8 movingLeft  = 0;
    uint8 movingRight = 0;

    int32 offset = 0;
#if RETRO_REV0U
    offset = useCollisionOffset ? COLLISION_OFFSET : 0;
#else
    offset = collisionOffset;
#endif

    if (collisionEntity->velocity.x >= 0) {
        movingRight           = 1;
        sensors[0].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.right);
        sensors[0].position.y = collisionEntity->position.y + offset;
    }

    if (collisionEntity->velocity.x <= 0) {
        movingLeft            = 1;
        sensors[1].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.left) - TO_FIXED(1);
        sensors[1].position.y = collisionEntity->position.y + offset;
    }

    // Bug Details:
    // using collision outer here results in a few slopes having some wonky collision if you jump at the on the wrong angles
    // v4 and prior uses inner box here instead, which works fine as far as I can tell...
    // Fix (I think, it may break something else?):
    // uncomment the 2 lines below and remove the two below that to get v4-like behaviour
    // sensors[2].pos.x = collisionEntity->position.x + TO_FIXED(collisionInner.left);
    // sensors[3].pos.x = collisionEntity->position.x + TO_FIXED(collisionInner.right);
    sensors[2].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.left) + TO_FIXED(1);
    sensors[3].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.right) - TO_FIXED(2);
    sensors[4].position.x = sensors[2].position.x;
    sensors[5].position.x = sensors[3].position.x;

    sensors[0].collided = false;
    sensors[1].collided = false;
    sensors[2].collided = false;
    sensors[3].collided = false;
    sensors[4].collided = false;
    sensors[5].collided = false;
    if (collisionEntity->velocity.y >= 0) {
        movingDown            = 1;
        sensors[2].position.y = collisionEntity->position.y + TO_FIXED(collisionOuter.bottom);
        sensors[3].position.y = collisionEntity->position.y + TO_FIXED(collisionOuter.bottom);
    }

    if (abs(collisionEntity->velocity.x) > TO_FIXED(1) || collisionEntity->velocity.y < 0) {
        movingUp              = 1;
        sensors[4].position.y = collisionEntity->position.y + TO_FIXED(collisionOuter.top) - TO_FIXED(1);
        sensors[5].position.y = collisionEntity->position.y + TO_FIXED(collisionOuter.top) - TO_FIXED(1);
    }

    int32 cnt   = (abs(collisionEntity->velocity.x) <= abs(collisionEntity->velocity.y) ? ((abs(collisionEntity->velocity.y) >> collisionMaskAir) + 1)
                                                                                        : (abs(collisionEntity->velocity.x) >> collisionMaskAir) + 1);
    int32 velX  = collisionEntity->velocity.x / cnt;
    int32 velY  = collisionEntity->velocity.y / cnt;
    int32 velX2 = collisionEntity->velocity.x - velX * (cnt - 1);
    int32 velY2 = collisionEntity->velocity.y - velY * (cnt - 1);
    while (cnt > 0) {
        if (cnt < 2) {
            velX = velX2;
            velY = velY2;
        }
        cnt--;

        if (movingRight == 1) {
            sensors[0].position.x += velX;
            sensors[0].position.y += velY;
            LWallCollision(&sensors[0]);

            if (sensors[0].collided) {
                movingRight = 2;
            }
#if !RETRO_REV0U
            else if (collisionEntity->velocity.x < TO_FIXED(2) && collisionOffset > 0) {
                sensors[0].position.y -= collisionOffset << 1;
                LWallCollision(&sensors[0]);

                if (sensors[0].collided)
                    movingRight = 2;
                sensors[0].position.y += collisionOffset << 1;
            }
#endif
        }

        if (movingLeft == 1) {
            sensors[1].position.x += velX;
            sensors[1].position.y += velY;
            RWallCollision(&sensors[1]);

            if (sensors[1].collided) {
                movingLeft = 2;
            }
#if !RETRO_REV0U
            else if (collisionEntity->velocity.x > -TO_FIXED(2) && collisionOffset > 0) {
                sensors[1].position.y -= collisionOffset << 1;
                RWallCollision(&sensors[1]);

                if (sensors[1].collided)
                    movingLeft = 2;
                sensors[1].position.y += collisionOffset << 1;
            }
#endif
        }

        if (movingRight == 2) {
            collisionEntity->velocity.x = 0;
            collisionEntity->groundVel  = 0;
            collisionEntity->position.x = sensors[0].position.x - TO_FIXED(collisionOuter.right);

            sensors[2].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.left) + TO_FIXED(1);
            sensors[3].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.right) - TO_FIXED(2);
            sensors[4].position.x = sensors[2].position.x;
            sensors[5].position.x = sensors[3].position.x;

            velX        = 0;
            velX2       = 0;
            movingRight = 3;
        }

        if (movingLeft == 2) {
            collisionEntity->velocity.x = 0;
            collisionEntity->groundVel  = 0;
            collisionEntity->position.x = sensors[1].position.x - TO_FIXED(collisionOuter.left) + TO_FIXED(1);

            sensors[2].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.left) + TO_FIXED(1);
            sensors[3].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.right) - TO_FIXED(2);
            sensors[4].position.x = sensors[2].position.x;
            sensors[5].position.x = sensors[3].position.x;

            velX       = 0;
            velX2      = 0;
            movingLeft = 3;
        }

        if (movingDown == 1) {
            for (int32 i = 2; i < 4; i++) {
                if (!sensors[i].collided) {
                    sensors[i].position.x += velX;
                    sensors[i].position.y += velY;
                    FloorCollision(&sensors[i]);
                }
            }

            if (sensors[2].collided || sensors[3].collided) {
                movingDown = 2;
                cnt        = 0;
            }
        }

        if (movingUp == 1) {
            for (int32 i = 4; i < 6; i++) {
                if (!sensors[i].collided) {
                    sensors[i].position.x += velX;
                    sensors[i].position.y += velY;
                    RoofCollision(&sensors[i]);
                }
            }

            if (sensors[4].collided || sensors[5].collided) {
                movingUp = 2;
                cnt      = 0;
            }
        }
    }

    if (movingRight < 2 && movingLeft < 2)
        collisionEntity->position.x += collisionEntity->velocity.x;

    if (movingUp < 2 && movingDown < 2) {
        collisionEntity->position.y += collisionEntity->velocity.y;
        return;
    }

    if (movingDown == 2) {
        collisionEntity->onGround = true;

        if (sensors[2].collided && sensors[3].collided) {
            if (sensors[2].position.y >= sensors[3].position.y) {
                collisionEntity->position.y = sensors[3].position.y - TO_FIXED(collisionOuter.bottom);
                collisionEntity->angle      = sensors[3].angle;
            }
            else {
                collisionEntity->position.y = sensors[2].position.y - TO_FIXED(collisionOuter.bottom);
                collisionEntity->angle      = sensors[2].angle;
            }
        }
        else if (sensors[2].collided) {
            collisionEntity->position.y = sensors[2].position.y - TO_FIXED(collisionOuter.bottom);
            collisionEntity->angle      = sensors[2].angle;
        }
        else if (sensors[3].collided) {
            collisionEntity->position.y = sensors[3].position.y - TO_FIXED(collisionOuter.bottom);
            collisionEntity->angle      = sensors[3].angle;
        }

        if (collisionEntity->angle > 0xA0 && collisionEntity->angle < 0xDE && collisionEntity->collisionMode != CMODE_LWALL) {
            collisionEntity->collisionMode = CMODE_LWALL;
            collisionEntity->position.x -= TO_FIXED(4);
        }

        if (collisionEntity->angle > 0x22 && collisionEntity->angle < 0x60 && collisionEntity->collisionMode != CMODE_RWALL) {
            collisionEntity->collisionMode = CMODE_RWALL;
            collisionEntity->position.x += TO_FIXED(4);
        }

        int32 speed = 0;
        if (collisionEntity->angle < 0x80) {
            if (collisionEntity->angle < 0x10) {
                speed = collisionEntity->velocity.x;
            }
            else if (collisionEntity->angle >= 0x20) {
                speed = (abs(collisionEntity->velocity.x) <= abs(collisionEntity->velocity.y) ? collisionEntity->velocity.y
                                                                                              : collisionEntity->velocity.x);
            }
            else {
                speed = (abs(collisionEntity->velocity.x) <= abs(collisionEntity->velocity.y >> 1) ? (collisionEntity->velocity.y >> 1)
                                                                                                   : collisionEntity->velocity.x);
            }
        }
        else if (collisionEntity->angle > 0xF0) {
            speed = collisionEntity->velocity.x;
        }
        else if (collisionEntity->angle <= 0xE0) {
            speed =
                (abs(collisionEntity->velocity.x) <= abs(collisionEntity->velocity.y) ? -collisionEntity->velocity.y : collisionEntity->velocity.x);
        }
        else {
            speed = (abs(collisionEntity->velocity.x) <= abs(collisionEntity->velocity.y >> 1) ? -(collisionEntity->velocity.y >> 1)
                                                                                               : collisionEntity->velocity.x);
        }

        if (speed < -TO_FIXED(24))
            speed = -TO_FIXED(24);

        if (speed > TO_FIXED(24))
            speed = TO_FIXED(24);

        collisionEntity->groundVel  = speed;
        collisionEntity->velocity.x = speed;
        collisionEntity->velocity.y = 0;
    }

    if (movingUp == 2) {
        int32 sensorAngle = 0;

        if (sensors[4].collided && sensors[5].collided) {
            if (sensors[4].position.y <= sensors[5].position.y) {
                collisionEntity->position.y = sensors[5].position.y - TO_FIXED(collisionOuter.top) + TO_FIXED(1);
                sensorAngle                 = sensors[5].angle;
            }
            else {
                collisionEntity->position.y = sensors[4].position.y - TO_FIXED(collisionOuter.top) + TO_FIXED(1);
                sensorAngle                 = sensors[4].angle;
            }
        }
        else if (sensors[4].collided) {
            collisionEntity->position.y = sensors[4].position.y - TO_FIXED(collisionOuter.top) + TO_FIXED(1);
            sensorAngle                 = sensors[4].angle;
        }
        else if (sensors[5].collided) {
            collisionEntity->position.y = sensors[5].position.y - TO_FIXED(collisionOuter.top) + TO_FIXED(1);
            sensorAngle                 = sensors[5].angle;
        }
        sensorAngle &= 0xFF;

        if (sensorAngle < 0x62) {
            if (collisionEntity->velocity.y < -abs(collisionEntity->velocity.x)) {
                collisionEntity->onGround      = true;
                collisionEntity->angle         = sensorAngle;
                collisionEntity->collisionMode = CMODE_RWALL;
                collisionEntity->position.x += TO_FIXED(4);
                collisionEntity->position.y -= TO_FIXED(2);

                collisionEntity->groundVel = collisionEntity->angle <= 0x60 ? collisionEntity->velocity.y : (collisionEntity->velocity.y >> 1);
            }
        }

        if (sensorAngle > 0x9E && sensorAngle < 0xC1) {
            if (collisionEntity->velocity.y < -abs(collisionEntity->velocity.x)) {
                collisionEntity->onGround      = true;
                collisionEntity->angle         = sensorAngle;
                collisionEntity->collisionMode = CMODE_LWALL;
                collisionEntity->position.x -= TO_FIXED(4);
                collisionEntity->position.y -= TO_FIXED(2);

                collisionEntity->groundVel = collisionEntity->angle >= 0xA0 ? -collisionEntity->velocity.y : -(collisionEntity->velocity.y >> 1);
            }
        }

        if (collisionEntity->velocity.y < 0)
            collisionEntity->velocity.y = 0;
    }
}
#if RETRO_REV0U
void RSDK::ProcessAirCollision_Up()
{
    uint8 movingDown  = 0;
    uint8 movingUp    = 0;
    uint8 movingLeft  = 0;
    uint8 movingRight = 0;

    int32 offset = useCollisionOffset ? -COLLISION_OFFSET : 0;

    if (collisionEntity->velocity.x >= 0) {
        movingRight           = 1;
        sensors[0].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.right);
        sensors[0].position.y = collisionEntity->position.y + offset;
    }

    if (collisionEntity->velocity.x <= 0) {
        movingLeft            = 1;
        sensors[1].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.left) - TO_FIXED(1);
        sensors[1].position.y = collisionEntity->position.y + offset;
    }

    // Bug Details:
    // using collision outer here results in a few slopes having some wonky collision if you jump at the on the wrong angles
    // v4 and prior uses inner box here instead, which works fine as far as I can tell...
    // Fix (I think, it may break something else?):
    // uncomment the 2 lines below and remove the two below that to get v4-like behaviour
    // sensors[2].pos.x = collisionEntity->position.x + TO_FIXED(collisionInner.left);
    // sensors[3].pos.x = collisionEntity->position.x + TO_FIXED(collisionInner.right);
    sensors[2].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.left) + TO_FIXED(1);
    sensors[3].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.right) - TO_FIXED(2);
    sensors[4].position.x = sensors[2].position.x;
    sensors[5].position.x = sensors[3].position.x;

    sensors[0].collided = false;
    sensors[1].collided = false;
    sensors[2].collided = false;
    sensors[3].collided = false;
    sensors[4].collided = false;
    sensors[5].collided = false;
    if (collisionEntity->velocity.y <= 0) {
        movingDown            = 1;
        sensors[4].position.y = collisionEntity->position.y + TO_FIXED(collisionOuter.top) - TO_FIXED(1);
        sensors[5].position.y = collisionEntity->position.y + TO_FIXED(collisionOuter.top) - TO_FIXED(1);
    }

    if (abs(collisionEntity->velocity.x) > TO_FIXED(1) || collisionEntity->velocity.y > 0) {
        movingUp              = 1;
        sensors[2].position.y = collisionEntity->position.y + TO_FIXED(collisionOuter.bottom);
        sensors[3].position.y = collisionEntity->position.y + TO_FIXED(collisionOuter.bottom);
    }

    int32 cnt   = (abs(collisionEntity->velocity.x) <= abs(collisionEntity->velocity.y) ? ((abs(collisionEntity->velocity.y) >> collisionMaskAir) + 1)
                                                                                        : (abs(collisionEntity->velocity.x) >> collisionMaskAir) + 1);
    int32 velX  = collisionEntity->velocity.x / cnt;
    int32 velY  = collisionEntity->velocity.y / cnt;
    int32 velX2 = collisionEntity->velocity.x - velX * (cnt - 1);
    int32 velY2 = collisionEntity->velocity.y - velY * (cnt - 1);
    while (cnt > 0) {
        if (cnt < 2) {
            velX = velX2;
            velY = velY2;
        }
        cnt--;

        if (movingRight == 1) {
            sensors[0].position.x += velX;
            sensors[0].position.y += velY;
            LWallCollision(&sensors[0]);

            if (sensors[0].collided) {
                movingRight = 2;
            }
#if !RETRO_REV0U
            else if (collisionEntity->velocity.x < TO_FIXED(2) && collisionOffset > 0) {
                sensors[0].position.y -= collisionOffset << 1;
                LWallCollision(&sensors[0]);

                if (sensors[0].collided)
                    movingRight = 2;
                sensors[0].position.y += collisionOffset << 1;
            }
#endif
        }

        if (movingLeft == 1) {
            sensors[1].position.x += velX;
            sensors[1].position.y += velY;
            RWallCollision(&sensors[1]);

            if (sensors[1].collided) {
                movingLeft = 2;
            }
#if !RETRO_REV0U
            else if (collisionEntity->velocity.x > -TO_FIXED(2) && collisionOffset > 0) {
                sensors[1].position.y -= collisionOffset << 1;
                RWallCollision(&sensors[1]);

                if (sensors[1].collided)
                    movingLeft = 2;
                sensors[1].position.y += collisionOffset << 1;
            }
#endif
        }

        if (movingRight == 2) {
            collisionEntity->velocity.x = 0;
            collisionEntity->groundVel  = 0;
            collisionEntity->position.x = sensors[0].position.x - TO_FIXED(collisionOuter.right);

            sensors[2].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.left) + TO_FIXED(1);
            sensors[3].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.right) - TO_FIXED(2);
            sensors[4].position.x = sensors[2].position.x;
            sensors[5].position.x = sensors[3].position.x;

            velX        = 0;
            velX2       = 0;
            movingRight = 3;
        }

        if (movingLeft == 2) {
            collisionEntity->velocity.x = 0;
            collisionEntity->groundVel  = 0;
            collisionEntity->position.x = sensors[1].position.x - TO_FIXED(collisionOuter.left) + TO_FIXED(1);

            sensors[2].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.left) + TO_FIXED(1);
            sensors[3].position.x = collisionEntity->position.x + TO_FIXED(collisionOuter.right) - TO_FIXED(2);
            sensors[4].position.x = sensors[2].position.x;
            sensors[5].position.x = sensors[3].position.x;

            velX       = 0;
            velX2      = 0;
            movingLeft = 3;
        }

        if (movingUp == 1) {
            for (int32 i = 2; i < 4; i++) {
                if (!sensors[i].collided) {
                    sensors[i].position.x += velX;
                    sensors[i].position.y += velY;
                    FloorCollision(&sensors[i]);
                }
            }

            if (sensors[2].collided || sensors[3].collided) {
                movingUp = 2;
                cnt      = 0;
            }
        }

        if (movingDown == 1) {
            for (int32 i = 4; i < 6; i++) {
                if (!sensors[i].collided) {
                    sensors[i].position.x += velX;
                    sensors[i].position.y += velY;
                    RoofCollision(&sensors[i]);
                }
            }

            if (sensors[4].collided || sensors[5].collided) {
                movingDown = 2;
                cnt        = 0;
            }
        }
    }

    if (movingRight < 2 && movingLeft < 2)
        collisionEntity->position.x += collisionEntity->velocity.x;

    if (movingUp < 2 && movingDown < 2) {
        collisionEntity->position.y += collisionEntity->velocity.y;
        return;
    }

    if (movingDown == 2) {
        collisionEntity->onGround = true;

        if (sensors[4].collided && sensors[5].collided) {
            if (sensors[4].position.y <= sensors[5].position.y) {
                collisionEntity->position.y = sensors[5].position.y - TO_FIXED(collisionOuter.top) + TO_FIXED(1);
                collisionEntity->angle      = sensors[5].angle;
            }
            else {
                collisionEntity->position.y = sensors[4].position.y - TO_FIXED(collisionOuter.top) + TO_FIXED(1);
                collisionEntity->angle      = sensors[4].angle;
            }
        }
        else if (sensors[4].collided) {
            collisionEntity->position.y = sensors[4].position.y - TO_FIXED(collisionOuter.top) + TO_FIXED(1);
            collisionEntity->angle      = sensors[4].angle;
        }
        else if (sensors[5].collided) {
            collisionEntity->position.y = sensors[5].position.y - TO_FIXED(collisionOuter.top) + TO_FIXED(1);
            collisionEntity->angle      = sensors[5].angle;
        }

        if (collisionEntity->angle > 0xA2 && collisionEntity->angle < 0xE0 && collisionEntity->collisionMode != CMODE_LWALL) {
            collisionEntity->collisionMode = CMODE_LWALL;
            collisionEntity->position.x -= TO_FIXED(4);
        }

        if (collisionEntity->angle > 0x20 && collisionEntity->angle < 0x5E && collisionEntity->collisionMode != CMODE_RWALL) {
            collisionEntity->collisionMode = CMODE_RWALL;
            collisionEntity->position.x += TO_FIXED(4);
        }

        int32 speed = 0;
        if (collisionEntity->angle >= 0x80) {
            if (collisionEntity->angle < 0x90) {
                speed = -collisionEntity->velocity.x;
            }
            else if (collisionEntity->angle >= 0xA0) {
                speed = (abs(collisionEntity->velocity.x) <= abs(collisionEntity->velocity.y) ? collisionEntity->velocity.y
                                                                                              : collisionEntity->velocity.x);
            }
            else {
                speed = (abs(collisionEntity->velocity.x) <= abs(collisionEntity->velocity.y >> 1) ? (collisionEntity->velocity.y >> 1)
                                                                                                   : collisionEntity->velocity.x);
            }
        }
        else if (collisionEntity->angle <= 0x70) {
            speed = collisionEntity->velocity.x;
        }
        else if (collisionEntity->angle <= 0x60) {
            speed =
                (abs(collisionEntity->velocity.x) <= abs(collisionEntity->velocity.y) ? -collisionEntity->velocity.y : collisionEntity->velocity.x);
        }
        else {
            speed = (abs(collisionEntity->velocity.x) <= abs(collisionEntity->velocity.y >> 1) ? -(collisionEntity->velocity.y >> 1)
                                                                                               : collisionEntity->velocity.x);
        }

        if (speed < -TO_FIXED(24))
            speed = -TO_FIXED(24);

        if (speed > TO_FIXED(24))
            speed = TO_FIXED(24);

        collisionEntity->groundVel  = speed;
        collisionEntity->velocity.x = speed;
        collisionEntity->velocity.y = 0;
    }

    if (movingUp == 2) {
        int32 sensorAngle = 0;

        if (sensors[2].collided && sensors[3].collided) {
            if (sensors[2].position.y >= sensors[3].position.y) {
                collisionEntity->position.y = sensors[3].position.y - TO_FIXED(collisionOuter.bottom);
                sensorAngle                 = sensors[3].angle;
            }
            else {
                collisionEntity->position.y = sensors[2].position.y - TO_FIXED(collisionOuter.bottom);
                sensorAngle                 = sensors[2].angle;
            }
        }
        else if (sensors[2].collided) {
            collisionEntity->position.y = sensors[2].position.y - TO_FIXED(collisionOuter.bottom);
            sensorAngle                 = sensors[2].angle;
        }
        else if (sensors[3].collided) {
            collisionEntity->position.y = sensors[3].position.y - TO_FIXED(collisionOuter.bottom);
            sensorAngle                 = sensors[3].angle;
        }
        sensorAngle &= 0xFF;

        if (sensorAngle >= 0x21 && sensorAngle <= 0x40) {
            if (collisionEntity->velocity.y > -abs(collisionEntity->velocity.x)) {
                collisionEntity->onGround      = true;
                collisionEntity->angle         = sensorAngle;
                collisionEntity->collisionMode = CMODE_RWALL;
                collisionEntity->position.x += TO_FIXED(4);
                collisionEntity->position.y -= TO_FIXED(2);

                collisionEntity->groundVel = collisionEntity->angle <= 0x20 ? collisionEntity->velocity.y : (collisionEntity->velocity.y >> 1);
            }
        }

        if (sensorAngle >= 0xC0 && sensorAngle <= 0xE2) {
            if (collisionEntity->velocity.y > -abs(collisionEntity->velocity.x)) {
                collisionEntity->onGround      = true;
                collisionEntity->angle         = sensorAngle;
                collisionEntity->collisionMode = CMODE_LWALL;
                collisionEntity->position.x -= TO_FIXED(4);
                collisionEntity->position.y -= TO_FIXED(2);

                collisionEntity->groundVel = collisionEntity->angle <= 0xE0 ? -collisionEntity->velocity.y : -(collisionEntity->velocity.y >> 1);
            }
        }

        if (collisionEntity->velocity.y > 0)
            collisionEntity->velocity.y = 0;
    }
}
#endif
void RSDK::ProcessPathGrip()
{
    int32 xVel = 0;
    int32 yVel = 0;

    sensors[4].position.x = collisionEntity->position.x;
    sensors[4].position.y = collisionEntity->position.y;
    for (int32 i = 0; i < 6; ++i) {
        sensors[i].angle    = collisionEntity->angle;
        sensors[i].collided = false;
    }
    SetPathGripSensors(sensors);

    int32 absSpeed  = abs(collisionEntity->groundVel);
    int32 checkDist = absSpeed >> 18;
    absSpeed &= 0x3FFFF;
    while (checkDist > -1) {
        if (checkDist >= 1) {
            xVel = cos256LookupTable[collisionEntity->angle] << 10;
            yVel = sin256LookupTable[collisionEntity->angle] << 10;
            checkDist--;
        }
        else {
            xVel      = absSpeed * cos256LookupTable[collisionEntity->angle] >> 8;
            yVel      = absSpeed * sin256LookupTable[collisionEntity->angle] >> 8;
            checkDist = -1;
        }

        if (collisionEntity->groundVel < 0) {
            xVel = -xVel;
            yVel = -yVel;
        }

        sensors[0].collided = false;
        sensors[1].collided = false;
        sensors[2].collided = false;
        sensors[4].position.x += xVel;
        sensors[4].position.y += yVel;
        int32 tileDistance = -1;

        switch (collisionEntity->collisionMode) {
            case CMODE_FLOOR: {
                sensors[3].position.x += xVel;
                sensors[3].position.y += yVel;

                if (collisionEntity->groundVel > 0) {
                    LWallCollision(&sensors[3]);
#if RETRO_REV0U
                    if (sensors[3].collided)
                        sensors[2].position.x = sensors[3].position.x - TO_FIXED(2);
#endif
                }

                if (collisionEntity->groundVel < 0) {
                    RWallCollision(&sensors[3]);
#if RETRO_REV0U
                    if (sensors[3].collided)
                        sensors[0].position.x = sensors[3].position.x + TO_FIXED(2);
#endif
                }

                if (sensors[3].collided) {
                    xVel      = 0;
                    checkDist = -1;
                }

                for (int32 i = 0; i < 3; i++) {
                    sensors[i].position.x += xVel;
                    sensors[i].position.y += yVel;
                    FindFloorPosition(&sensors[i]);
                }

                tileDistance = -1;
                for (int32 i = 0; i < 3; i++) {
                    if (tileDistance > -1) {
                        if (sensors[i].collided) {
                            if (sensors[i].position.y < sensors[tileDistance].position.y)
                                tileDistance = i;

                            if (sensors[i].position.y == sensors[tileDistance].position.y && (sensors[i].angle < 0x08 || sensors[i].angle > 0xF8))
                                tileDistance = i;
                        }
                    }
                    else if (sensors[i].collided)
                        tileDistance = i;
                }

                if (tileDistance <= -1) {
                    checkDist = -1;
                }
                else {
                    sensors[0].position.y = sensors[tileDistance].position.y;
                    sensors[0].angle      = sensors[tileDistance].angle;

                    sensors[1].position.y = sensors[0].position.y;
                    sensors[1].angle      = sensors[0].angle;

                    sensors[2].position.y = sensors[0].position.y;
                    sensors[2].angle      = sensors[0].angle;

                    sensors[4].position.x = sensors[1].position.x;
                    sensors[4].position.y = sensors[0].position.y - TO_FIXED(collisionOuter.bottom);
                }

                if (sensors[0].angle < 0xDE && sensors[0].angle > 0x80)
                    collisionEntity->collisionMode = CMODE_LWALL;
                if (sensors[0].angle > 0x22 && sensors[0].angle < 0x80)
                    collisionEntity->collisionMode = CMODE_RWALL;
                break;
            }

            case CMODE_LWALL: {
                sensors[3].position.x += xVel;
                sensors[3].position.y += yVel;

                if (collisionEntity->groundVel > 0)
                    RoofCollision(&sensors[3]);

                if (collisionEntity->groundVel < 0)
                    FloorCollision(&sensors[3]);

                if (sensors[3].collided) {
                    yVel      = 0;
                    checkDist = -1;
                }

                for (int32 i = 0; i < 3; i++) {
                    sensors[i].position.x += xVel;
                    sensors[i].position.y += yVel;
                    FindLWallPosition(&sensors[i]);
                }

                tileDistance = -1;
                for (int32 i = 0; i < 3; i++) {
                    if (tileDistance > -1) {
                        if (sensors[i].position.x < sensors[tileDistance].position.x && sensors[i].collided) {
                            tileDistance = i;
                        }
                    }
                    else if (sensors[i].collided) {
                        tileDistance = i;
                    }
                }

                if (tileDistance <= -1) {
                    checkDist = -1;
                }
                else {
                    sensors[0].position.x = sensors[tileDistance].position.x;
                    sensors[0].angle      = sensors[tileDistance].angle;

                    sensors[1].position.x = sensors[0].position.x;
                    sensors[1].angle      = sensors[0].angle;

                    sensors[2].position.x = sensors[0].position.x;
                    sensors[2].angle      = sensors[0].angle;

                    sensors[4].position.x = sensors[1].position.x - TO_FIXED(collisionOuter.bottom);
                    sensors[4].position.y = sensors[1].position.y;
                }

                if (sensors[0].angle > 0xE2)
                    collisionEntity->collisionMode = CMODE_FLOOR;

                if (sensors[0].angle < 0x9E)
                    collisionEntity->collisionMode = CMODE_ROOF;
                break;
            }

            case CMODE_ROOF: {
                sensors[3].position.x += xVel;
                sensors[3].position.y += yVel;

                if (collisionEntity->groundVel > 0) {
                    RWallCollision(&sensors[3]);
#if RETRO_REV0U
                    if (sensors[3].collided)
                        sensors[2].position.x = sensors[3].position.x + TO_FIXED(2);
#endif
                }

                if (collisionEntity->groundVel < 0) {
                    LWallCollision(&sensors[3]);
#if RETRO_REV0U
                    if (sensors[3].collided)
                        sensors[0].position.x = sensors[3].position.x - TO_FIXED(2);
#endif
                }

                if (sensors[3].collided) {
                    xVel      = 0;
                    checkDist = -1;
                }

                for (int32 i = 0; i < 3; i++) {
                    sensors[i].position.x += xVel;
                    sensors[i].position.y += yVel;
                    FindRoofPosition(&sensors[i]);
                }

                tileDistance = -1;
                for (int32 i = 0; i < 3; i++) {
                    if (tileDistance > -1) {
                        if (sensors[i].position.y > sensors[tileDistance].position.y && sensors[i].collided) {
                            tileDistance = i;
                        }
                    }
                    else if (sensors[i].collided) {
                        tileDistance = i;
                    }
                }

                if (tileDistance <= -1) {
                    checkDist = -1;
                }
                else {
                    sensors[0].position.y = sensors[tileDistance].position.y;
                    sensors[0].angle      = sensors[tileDistance].angle;

                    sensors[1].position.y = sensors[0].position.y;
                    sensors[1].angle      = sensors[0].angle;

                    sensors[2].position.y = sensors[0].position.y;
                    sensors[2].angle      = sensors[0].angle;

                    sensors[4].position.x = sensors[1].position.x;
                    sensors[4].position.y = sensors[0].position.y + TO_FIXED(collisionOuter.bottom) + TO_FIXED(1);
                }

                if (sensors[0].angle > 0xA2)
                    collisionEntity->collisionMode = CMODE_LWALL;
                if (sensors[0].angle < 0x5E)
                    collisionEntity->collisionMode = CMODE_RWALL;
                break;
            }

            case CMODE_RWALL: {
                sensors[3].position.x += xVel;
                sensors[3].position.y += yVel;

                if (collisionEntity->groundVel > 0)
                    FloorCollision(&sensors[3]);

                if (collisionEntity->groundVel < 0)
                    RoofCollision(&sensors[3]);

                if (sensors[3].collided) {
                    yVel      = 0;
                    checkDist = -1;
                }

                for (int32 i = 0; i < 3; i++) {
                    sensors[i].position.x += xVel;
                    sensors[i].position.y += yVel;
                    FindRWallPosition(&sensors[i]);
                }

                tileDistance = -1;
                for (int32 i = 0; i < 3; i++) {
                    if (tileDistance > -1) {
                        if (sensors[i].position.x > sensors[tileDistance].position.x && sensors[i].collided) {
                            tileDistance = i;
                        }
                    }
                    else if (sensors[i].collided) {
                        tileDistance = i;
                    }
                }

                if (tileDistance <= -1) {
                    checkDist = -1;
                }
                else {
                    sensors[0].position.x = sensors[tileDistance].position.x;
                    sensors[0].angle      = sensors[tileDistance].angle;

                    sensors[1].position.x = sensors[0].position.x;
                    sensors[1].angle      = sensors[0].angle;

                    sensors[2].position.x = sensors[0].position.x;
                    sensors[2].angle      = sensors[0].angle;

                    sensors[4].position.x = sensors[1].position.x + TO_FIXED(collisionOuter.bottom) + TO_FIXED(1);
                    sensors[4].position.y = sensors[1].position.y;
                }

                if (sensors[0].angle < 0x1E)
                    collisionEntity->collisionMode = CMODE_FLOOR;
                if (sensors[0].angle > 0x62)
                    collisionEntity->collisionMode = CMODE_ROOF;
                break;
            }
        }

        if (tileDistance != -1)
            collisionEntity->angle = sensors[0].angle;

        if (!sensors[3].collided)
            SetPathGripSensors(sensors);
        else
            checkDist = -2;
    }

#if RETRO_REV0U
    int32 newCollisionMode = collisionEntity->tileCollisions == TILECOLLISION_DOWN ? CMODE_FLOOR : CMODE_ROOF;
    int32 newAngle         = newCollisionMode << 6;
#else
    int32 newCollisionMode = CMODE_FLOOR;
    int32 newAngle         = newCollisionMode << 6;
#endif

    switch (collisionEntity->collisionMode) {
        case CMODE_FLOOR: {
            if (sensors[0].collided || sensors[1].collided || sensors[2].collided) {
                collisionEntity->angle = sensors[0].angle;

                if (!sensors[3].collided) {
                    collisionEntity->position.x = sensors[4].position.x;
                }
                else {
                    if (collisionEntity->groundVel > 0)
                        collisionEntity->position.x = sensors[3].position.x - TO_FIXED(collisionOuter.right);

                    if (collisionEntity->groundVel < 0)
                        collisionEntity->position.x = sensors[3].position.x - TO_FIXED(collisionOuter.left) + TO_FIXED(1);

                    collisionEntity->groundVel  = 0;
                    collisionEntity->velocity.x = 0;
                }

                collisionEntity->position.y = sensors[4].position.y;
            }
            else {
                collisionEntity->onGround      = false;
                collisionEntity->collisionMode = newCollisionMode;
                collisionEntity->velocity.x    = cos256LookupTable[collisionEntity->angle] * collisionEntity->groundVel >> 8;
                collisionEntity->velocity.y    = sin256LookupTable[collisionEntity->angle] * collisionEntity->groundVel >> 8;
                if (collisionEntity->velocity.y < -TO_FIXED(16))
                    collisionEntity->velocity.y = -TO_FIXED(16);

                if (collisionEntity->velocity.y > TO_FIXED(16))
                    collisionEntity->velocity.y = TO_FIXED(16);

                collisionEntity->groundVel = collisionEntity->velocity.x;
                collisionEntity->angle     = newAngle;
                if (!sensors[3].collided) {
                    collisionEntity->position.x += collisionEntity->velocity.x;
                }
                else {
                    if (collisionEntity->groundVel > 0)
                        collisionEntity->position.x = sensors[3].position.x - TO_FIXED(collisionOuter.right);
                    if (collisionEntity->groundVel < 0)
                        collisionEntity->position.x = sensors[3].position.x - TO_FIXED(collisionOuter.left) + TO_FIXED(1);

                    collisionEntity->groundVel  = 0;
                    collisionEntity->velocity.x = 0;
                }

                collisionEntity->position.y += collisionEntity->velocity.y;
            }
            break;
        }

        case CMODE_LWALL: {
            if (sensors[0].collided || sensors[1].collided || sensors[2].collided) {
                collisionEntity->angle = sensors[0].angle;
            }
            else {
                collisionEntity->onGround      = false;
                collisionEntity->collisionMode = newCollisionMode;
                collisionEntity->velocity.x    = cos256LookupTable[collisionEntity->angle] * collisionEntity->groundVel >> 8;
                collisionEntity->velocity.y    = sin256LookupTable[collisionEntity->angle] * collisionEntity->groundVel >> 8;

                if (collisionEntity->velocity.y < -TO_FIXED(16))
                    collisionEntity->velocity.y = -TO_FIXED(16);

                if (collisionEntity->velocity.y > TO_FIXED(16))
                    collisionEntity->velocity.y = TO_FIXED(16);

                collisionEntity->groundVel = collisionEntity->velocity.x;
                collisionEntity->angle     = newAngle;
            }

            if (!sensors[3].collided) {
                collisionEntity->position.x = sensors[4].position.x;
                collisionEntity->position.y = sensors[4].position.y;
            }
            else {
                if (collisionEntity->groundVel > 0)
                    collisionEntity->position.y = sensors[3].position.y + TO_FIXED(collisionOuter.right) + TO_FIXED(1);

                if (collisionEntity->groundVel < 0)
                    collisionEntity->position.y = sensors[3].position.y - TO_FIXED(collisionOuter.left);

                collisionEntity->groundVel  = 0;
                collisionEntity->position.x = sensors[4].position.x;
            }
            break;
        }

        case CMODE_ROOF: {
            if (sensors[0].collided || sensors[1].collided || sensors[2].collided) {
                collisionEntity->angle = sensors[0].angle;

                if (!sensors[3].collided) {
                    collisionEntity->position.x = sensors[4].position.x;
                }
                else {
                    if (collisionEntity->groundVel > 0)
                        collisionEntity->position.x = sensors[3].position.x + TO_FIXED(collisionOuter.right);

                    if (collisionEntity->groundVel < 0)
                        collisionEntity->position.x = sensors[3].position.x + TO_FIXED(collisionOuter.left) - TO_FIXED(1);

                    collisionEntity->groundVel = 0;
                }
            }
            else {
                collisionEntity->onGround      = false;
                collisionEntity->collisionMode = newCollisionMode;
                collisionEntity->velocity.x    = cos256LookupTable[collisionEntity->angle] * collisionEntity->groundVel >> 8;
                collisionEntity->velocity.y    = sin256LookupTable[collisionEntity->angle] * collisionEntity->groundVel >> 8;

                if (collisionEntity->velocity.y < -TO_FIXED(16))
                    collisionEntity->velocity.y = -TO_FIXED(16);

                if (collisionEntity->velocity.y > TO_FIXED(16))
                    collisionEntity->velocity.y = TO_FIXED(16);

                collisionEntity->angle     = newAngle;
                collisionEntity->groundVel = collisionEntity->velocity.x;

                if (!sensors[3].collided) {
                    collisionEntity->position.x += collisionEntity->velocity.x;
                }
                else {
                    if (collisionEntity->groundVel > 0)
                        collisionEntity->position.x = sensors[3].position.x - TO_FIXED(collisionOuter.right);

                    if (collisionEntity->groundVel < 0)
                        collisionEntity->position.x = sensors[3].position.x - TO_FIXED(collisionOuter.left) + TO_FIXED(1);

                    collisionEntity->groundVel = 0;
                }
            }
            collisionEntity->position.y = sensors[4].position.y;
            break;
        }

        case CMODE_RWALL: {
            if (sensors[0].collided || sensors[1].collided || sensors[2].collided) {
                collisionEntity->angle = sensors[0].angle;
            }
            else {
                collisionEntity->onGround      = false;
                collisionEntity->collisionMode = newCollisionMode;
                collisionEntity->velocity.x    = cos256LookupTable[collisionEntity->angle] * collisionEntity->groundVel >> 8;
                collisionEntity->velocity.y    = sin256LookupTable[collisionEntity->angle] * collisionEntity->groundVel >> 8;

                if (collisionEntity->velocity.y < -TO_FIXED(16))
                    collisionEntity->velocity.y = -TO_FIXED(16);

                if (collisionEntity->velocity.y > TO_FIXED(16))
                    collisionEntity->velocity.y = TO_FIXED(16);

                collisionEntity->groundVel = collisionEntity->velocity.x;
                collisionEntity->angle     = newAngle;
            }

            if (!sensors[3].collided) {
                collisionEntity->position.x = sensors[4].position.x;
                collisionEntity->position.y = sensors[4].position.y;
            }
            else {
                if (collisionEntity->groundVel > 0)
                    collisionEntity->position.y = sensors[3].position.y - TO_FIXED(collisionOuter.right);

                if (collisionEntity->groundVel < 0)
                    collisionEntity->position.y = sensors[3].position.y - TO_FIXED(collisionOuter.left) + TO_FIXED(1);

                collisionEntity->groundVel  = 0;
                collisionEntity->position.x = sensors[4].position.x;
            }
            break;
        }

        default: break;
    }
}

void RSDK::SetPathGripSensors(CollisionSensor *sensors)
{
    int32 offset = 0;
#if RETRO_REV0U
    offset = useCollisionOffset ? COLLISION_OFFSET : 0;
#else
    offset = collisionOffset;
#endif

    switch (collisionEntity->collisionMode) {
        case CMODE_FLOOR:
            sensors[0].position.y = sensors[4].position.y + TO_FIXED(collisionOuter.bottom);
            sensors[1].position.y = sensors[4].position.y + TO_FIXED(collisionOuter.bottom);
            sensors[2].position.y = sensors[4].position.y + TO_FIXED(collisionOuter.bottom);
            sensors[3].position.y = sensors[4].position.y + offset;

            sensors[0].position.x = sensors[4].position.x + TO_FIXED(collisionInner.left) - TO_FIXED(1);
            sensors[1].position.x = sensors[4].position.x;
            sensors[2].position.x = sensors[4].position.x + TO_FIXED(collisionInner.right);
            if (collisionEntity->groundVel <= 0)
                sensors[3].position.x = sensors[4].position.x + TO_FIXED(collisionOuter.left) - TO_FIXED(1);
            else
                sensors[3].position.x = sensors[4].position.x + TO_FIXED(collisionOuter.right);
            break;

        case CMODE_LWALL:
            sensors[0].position.x = sensors[4].position.x + TO_FIXED(collisionOuter.bottom);
            sensors[1].position.x = sensors[4].position.x + TO_FIXED(collisionOuter.bottom);
            sensors[2].position.x = sensors[4].position.x + TO_FIXED(collisionOuter.bottom);
            sensors[3].position.x = sensors[4].position.x;

            sensors[0].position.y = sensors[4].position.y + TO_FIXED(collisionInner.left) - TO_FIXED(1);
            sensors[1].position.y = sensors[4].position.y;
            sensors[2].position.y = sensors[4].position.y + TO_FIXED(collisionInner.right);
            if (collisionEntity->groundVel <= 0)
                sensors[3].position.y = sensors[4].position.y - TO_FIXED(collisionOuter.left);
            else
                sensors[3].position.y = sensors[4].position.y - TO_FIXED(collisionOuter.right) - TO_FIXED(1);
            break;

        case CMODE_ROOF:
            sensors[0].position.y = sensors[4].position.y - TO_FIXED(collisionOuter.bottom) - TO_FIXED(1);
            sensors[1].position.y = sensors[4].position.y - TO_FIXED(collisionOuter.bottom) - TO_FIXED(1);
            sensors[2].position.y = sensors[4].position.y - TO_FIXED(collisionOuter.bottom) - TO_FIXED(1);
            sensors[3].position.y = sensors[4].position.y - offset;

            sensors[0].position.x = sensors[4].position.x + TO_FIXED(collisionInner.left) - TO_FIXED(1);
            sensors[1].position.x = sensors[4].position.x;
            sensors[2].position.x = sensors[4].position.x + TO_FIXED(collisionInner.right);
            if (collisionEntity->groundVel <= 0)
                sensors[3].position.x = sensors[4].position.x - TO_FIXED(collisionOuter.left);
            else
                sensors[3].position.x = sensors[4].position.x - TO_FIXED(collisionOuter.right) - TO_FIXED(1);
            break;

        case CMODE_RWALL:
            sensors[0].position.x = sensors[4].position.x - TO_FIXED(collisionOuter.bottom) - TO_FIXED(1);
            sensors[1].position.x = sensors[4].position.x - TO_FIXED(collisionOuter.bottom) - TO_FIXED(1);
            sensors[2].position.x = sensors[4].position.x - TO_FIXED(collisionOuter.bottom) - TO_FIXED(1);
            sensors[3].position.x = sensors[4].position.x;

            sensors[0].position.y = sensors[4].position.y + TO_FIXED(collisionInner.left) - TO_FIXED(1);
            sensors[1].position.y = sensors[4].position.y;
            sensors[2].position.y = sensors[4].position.y + TO_FIXED(collisionInner.right);
            if (collisionEntity->groundVel <= 0)
                sensors[3].position.y = sensors[4].position.y + TO_FIXED(collisionOuter.left) - TO_FIXED(1);
            else
                sensors[3].position.y = sensors[4].position.y + TO_FIXED(collisionOuter.right);
            break;

        default: break;
    }
}

void RSDK::FindFloorPosition(CollisionSensor *sensor)
{
    int32 posX = FROM_FIXED(sensor->position.x);
    int32 posY = FROM_FIXED(sensor->position.y);

    int32 solid = 0;
#if RETRO_REV0U
    if (collisionEntity->tileCollisions == TILECOLLISION_DOWN)
        solid = collisionEntity->collisionPlane ? (1 << 14) : (1 << 12);
    else
        solid = collisionEntity->collisionPlane ? (1 << 15) : (1 << 13);
#else
    solid = collisionEntity->collisionPlane ? (1 << 14) : (1 << 12);
#endif

    int32 startY = posY;

    for (int32 l = 0, layerID = 1; l < LAYER_COUNT; ++l, layerID <<= 1) {
        if (collisionEntity->collisionLayers & layerID) {
            TileLayer *layer = &tileLayers[l];
            int32 colX       = posX - layer->position.x;
            int32 colY       = posY - layer->position.y;
            int32 cy         = (colY & -TILE_SIZE) - TILE_SIZE;
            if (colX >= 0 && colX < TILE_SIZE * layer->xsize) {
                for (int32 i = 0; i < 3; ++i) {
                    if (cy >= 0 && cy < TILE_SIZE * layer->ysize) {
                        uint16 tile = layer->layout[(colX / TILE_SIZE) + ((cy / TILE_SIZE) << layer->widthShift)];

                        if (tile < 0xFFFF && tile & solid) {
                            int32 mask      = collisionMasks[collisionEntity->collisionPlane][tile & 0xFFF].floorMasks[colX & 0xF];
                            int32 ty        = cy + mask;
                            int32 tileAngle = tileInfo[collisionEntity->collisionPlane][tile & 0xFFF].floorAngle;

                            if (mask < 0xFF) {
                                if (!sensor->collided || startY >= ty) {
                                    if (abs(colY - ty) <= collisionTolerance) {
#if RETRO_REV0U
#if !RETRO_USE_ORIGINAL_CODE
                                        if (abs(sensor->angle - tileAngle) <= TILE_SIZE * 2 // * 3 causes some issues in certain tiles
#else
                                        if (abs(sensor->angle - tileAngle) <= TILE_SIZE * 3
#endif
                                            || abs(sensor->angle - tileAngle + 0x100) <= floorAngleTolerance
                                            || abs(sensor->angle - tileAngle - 0x100) <= floorAngleTolerance) {
#else
                                        if (abs(sensor->angle - tileAngle) <= 0x20 || abs(sensor->angle - tileAngle + 0x100) <= 0x20
                                            || abs(sensor->angle - tileAngle - 0x100) <= 0x20) {
#endif
                                            sensor->collided   = true;
                                            sensor->angle      = tileAngle;
                                            sensor->position.y = TO_FIXED(ty + layer->position.y);
                                            startY             = ty;
                                            i                  = 3;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    cy += TILE_SIZE;
                }
            }

            posX = layer->position.x + colX;
            posY = layer->position.y + colY;
        }
    }
}
void RSDK::FindLWallPosition(CollisionSensor *sensor)
{
    int32 posX = FROM_FIXED(sensor->position.x);
    int32 posY = FROM_FIXED(sensor->position.y);

    int32 solid = collisionEntity->collisionPlane ? ((1 << 14) | (1 << 15)) : ((1 << 12) | (1 << 13));

    int32 startX = posX;

    for (int32 l = 0, layerID = 1; l < LAYER_COUNT; ++l, layerID <<= 1) {
        if (collisionEntity->collisionLayers & layerID) {
            TileLayer *layer = &tileLayers[l];
            int32 colX       = posX - layer->position.x;
            int32 colY       = posY - layer->position.y;
            int32 cx         = (colX & -TILE_SIZE) - TILE_SIZE;
            if (colY >= 0 && colY < TILE_SIZE * layer->ysize) {
                for (int32 i = 0; i < 3; ++i) {
                    if (cx >= 0 && cx < TILE_SIZE * layer->xsize) {
                        uint16 tile = layer->layout[(cx / TILE_SIZE) + ((colY / TILE_SIZE) << layer->widthShift)];

                        if (tile < 0xFFFF && tile & solid) {
                            int32 mask      = collisionMasks[collisionEntity->collisionPlane][tile & 0xFFF].lWallMasks[colY & 0xF];
                            int32 tx        = cx + mask;
                            int32 tileAngle = tileInfo[collisionEntity->collisionPlane][tile & 0xFFF].lWallAngle;

                            if (mask < 0xFF) {
                                if (!sensor->collided || startX >= tx) {
                                    if (abs(colX - tx) <= collisionTolerance && abs(sensor->angle - tileAngle) <= wallAngleTolerance) {
                                        sensor->collided   = true;
                                        sensor->angle      = tileAngle;
                                        sensor->position.x = TO_FIXED(tx + layer->position.x);
                                        startX             = tx;
                                        i                  = 3;
                                    }
                                }
                            }
                        }
                    }

                    cx += TILE_SIZE;
                }
            }

            posX = layer->position.x + colX;
            posY = layer->position.y + colY;
        }
    }
}
void RSDK::FindRoofPosition(CollisionSensor *sensor)
{
    int32 posX = FROM_FIXED(sensor->position.x);
    int32 posY = FROM_FIXED(sensor->position.y);

    int32 solid = 0;
#if RETRO_REV0U
    if (collisionEntity->tileCollisions == TILECOLLISION_DOWN)
        solid = collisionEntity->collisionPlane ? (1 << 15) : (1 << 13);
    else
        solid = collisionEntity->collisionPlane ? (1 << 14) : (1 << 12);
#else
    solid = collisionEntity->collisionPlane ? (1 << 15) : (1 << 13);
#endif

    int32 startY = posY;

    for (int32 l = 0, layerID = 1; l < LAYER_COUNT; ++l, layerID <<= 1) {
        if (collisionEntity->collisionLayers & layerID) {
            TileLayer *layer = &tileLayers[l];
            int32 colX       = posX - layer->position.x;
            int32 colY       = posY - layer->position.y;
            int32 cy         = (colY & -TILE_SIZE) + TILE_SIZE;

            if (colX >= 0 && colX < TILE_SIZE * layer->xsize) {
                for (int32 i = 0; i < 3; ++i) {
                    if (cy >= 0 && cy < TILE_SIZE * layer->ysize) {
                        uint16 tile = layer->layout[(colX / TILE_SIZE) + ((cy / TILE_SIZE) << layer->widthShift)];

                        if (tile < 0xFFFF && tile & solid) {
                            int32 mask      = collisionMasks[collisionEntity->collisionPlane][tile & 0xFFF].roofMasks[colX & 0xF];
                            int32 ty        = cy + mask;
                            int32 tileAngle = tileInfo[collisionEntity->collisionPlane][tile & 0xFFF].roofAngle;

                            if (mask < 0xFF) {
                                if (!sensor->collided || startY <= ty) {
                                    if (abs(colY - ty) <= collisionTolerance && abs(sensor->angle - tileAngle) <= roofAngleTolerance) {
                                        sensor->collided   = true;
                                        sensor->angle      = tileAngle;
                                        sensor->position.y = TO_FIXED(ty + layer->position.y);
                                        startY             = ty;
                                        i                  = 3;
                                    }
                                }
                            }
                        }
                    }

                    cy -= TILE_SIZE;
                }
            }

            posX = layer->position.x + colX;
            posY = layer->position.y + colY;
        }
    }
}
void RSDK::FindRWallPosition(CollisionSensor *sensor)
{
    int32 posX = FROM_FIXED(sensor->position.x);
    int32 posY = FROM_FIXED(sensor->position.y);

    int32 solid = collisionEntity->collisionPlane ? ((1 << 14) | (1 << 15)) : ((1 << 12) | (1 << 13));

    int32 startX = posX;

    for (int32 l = 0, layerID = 1; l < LAYER_COUNT; ++l, layerID <<= 1) {
        if (collisionEntity->collisionLayers & layerID) {
            TileLayer *layer = &tileLayers[l];
            int32 colX       = posX - layer->position.x;
            int32 colY       = posY - layer->position.y;
            int32 cx         = (colX & -TILE_SIZE) + TILE_SIZE;

            if (colY >= 0 && colY < TILE_SIZE * layer->ysize) {
                for (int32 i = 0; i < 3; ++i) {
                    if (cx >= 0 && cx < TILE_SIZE * layer->xsize) {
                        uint16 tile = layer->layout[(cx / TILE_SIZE) + ((colY / TILE_SIZE) << layer->widthShift)];

                        if (tile < 0xFFFF && tile & solid) {
                            int32 mask      = collisionMasks[collisionEntity->collisionPlane][tile & 0xFFF].rWallMasks[colY & 0xF];
                            int32 tx        = cx + mask;
                            int32 tileAngle = tileInfo[collisionEntity->collisionPlane][tile & 0xFFF].rWallAngle;

                            if (mask < 0xFF) {
                                if (!sensor->collided || startX <= tx) {
                                    if (abs(colX - tx) <= collisionTolerance && abs(sensor->angle - tileAngle) <= wallAngleTolerance) {
                                        sensor->collided   = true;
                                        sensor->angle      = tileAngle;
                                        sensor->position.x = TO_FIXED(tx + layer->position.x);
                                        startX             = tx;
                                        i                  = 3;
                                    }
                                }
                            }
                        }
                    }

                    cx -= TILE_SIZE;
                }
            }

            posX = layer->position.x + colX;
            posY = layer->position.y + colY;
        }
    }
}

void RSDK::FloorCollision(CollisionSensor *sensor)
{
    int32 posX = FROM_FIXED(sensor->position.x);
    int32 posY = FROM_FIXED(sensor->position.y);

    int32 solid = 0;
#if RETRO_REV0U
    if (collisionEntity->tileCollisions == TILECOLLISION_DOWN)
        solid = collisionEntity->collisionPlane ? (1 << 14) : (1 << 12);
    else
        solid = collisionEntity->collisionPlane ? (1 << 15) : (1 << 13);
#else
    solid = collisionEntity->collisionPlane ? (1 << 14) : (1 << 12);
#endif

#if RETRO_REV0U
    int32 collideAngle = 0;
    int32 collidePos   = 0x7FFFFFFF;
#endif

    for (int32 l = 0, layerID = 1; l < LAYER_COUNT; ++l, layerID <<= 1) {
        if (collisionEntity->collisionLayers & layerID) {
            TileLayer *layer = &tileLayers[l];
            int32 colX       = posX - layer->position.x;
            int32 colY       = posY - layer->position.y;
            int32 cy         = (colY & -TILE_SIZE) - TILE_SIZE;

            if (colX >= 0 && colX < TILE_SIZE * layer->xsize) {
#if RETRO_REV0U
                int32 stepCount = 2;
#else
                int32 stepCount = 3;
#endif
                for (int32 i = 0; i < stepCount; ++i) {
                    int32 step = TILE_SIZE;

                    if (cy >= 0 && cy < TILE_SIZE * layer->ysize) {
                        uint16 tile = layer->layout[(colX / TILE_SIZE) + ((cy / TILE_SIZE) << layer->widthShift)];
                        if (tile < 0xFFFF && tile & solid) {
                            int32 mask = collisionMasks[collisionEntity->collisionPlane][tile & 0xFFF].floorMasks[colX & 0xF];
#if RETRO_REV0U
                            int32 ty = layer->position.y + cy + mask;
#else
                            int32 ty = cy + mask;
#endif
                            if (mask < 0xFF) {
#if RETRO_REV0U
                                step = -TILE_SIZE;
                                if (colY < collidePos) {
                                    collideAngle = tileInfo[collisionEntity->collisionPlane][tile & 0xFFF].floorAngle;
                                    collidePos   = ty;
                                    i            = stepCount;
#else
                                if (colY >= ty) {
                                    if (abs(colY - ty) <= collisionMinimumDistance) {
                                        sensor->collided   = true;
                                        sensor->angle      = tileInfo[collisionEntity->collisionPlane][tile & 0xFFF].floorAngle;
                                        sensor->position.y = TO_FIXED(ty + layer->position.y);
                                        i                  = stepCount;
                                    }
#endif
                                }
                            }
                        }
                    }

                    cy += step;
                }
            }

            posX = layer->position.x + colX;
            posY = layer->position.y + colY;
        }
    }

#if RETRO_REV0U
    if (collidePos != 0x7FFFFFFF) {
        int32 collideDist = sensor->position.y - TO_FIXED(collidePos);
        if (sensor->position.y >= TO_FIXED(collidePos) && collideDist <= collisionMinimumDistance) {
            sensor->angle      = collideAngle;
            sensor->position.y = TO_FIXED(collidePos);
            sensor->collided   = true;
        }
    }
#endif
}
void RSDK::LWallCollision(CollisionSensor *sensor)
{
    int32 posX = FROM_FIXED(sensor->position.x);
    int32 posY = FROM_FIXED(sensor->position.y);

    int32 solid = collisionEntity->collisionPlane ? (1 << 15) : (1 << 13);

    for (int32 l = 0, layerID = 1; l < LAYER_COUNT; ++l, layerID <<= 1) {
        if (collisionEntity->collisionLayers & layerID) {
            TileLayer *layer = &tileLayers[l];
            int32 colX       = posX - layer->position.x;
            int32 colY       = posY - layer->position.y;
            int32 cx         = (colX & -TILE_SIZE) - TILE_SIZE;

            if (colY >= 0 && colY < TILE_SIZE * layer->ysize) {
                for (int32 i = 0; i < 3; ++i) {
                    if (cx >= 0 && cx < TILE_SIZE * layer->xsize) {
                        uint16 tile = layer->layout[(cx / TILE_SIZE) + ((colY / TILE_SIZE) << layer->widthShift)];

                        if (tile < 0xFFFF && tile & solid) {
                            int32 mask = collisionMasks[collisionEntity->collisionPlane][tile & 0xFFF].lWallMasks[colY & 0xF];
                            int32 tx   = cx + mask;
                            if (mask < 0xFF && colX >= tx && abs(colX - tx) <= 14) {
                                sensor->collided   = true;
                                sensor->angle      = tileInfo[collisionEntity->collisionPlane][tile & 0xFFF].lWallAngle;
                                sensor->position.x = TO_FIXED(tx + layer->position.x);
                                i                  = 3;
                            }
                        }
                    }

                    cx += TILE_SIZE;
                }
            }

            posX = layer->position.x + colX;
            posY = layer->position.y + colY;
        }
    }
}
void RSDK::RoofCollision(CollisionSensor *sensor)
{
    int32 posX = FROM_FIXED(sensor->position.x);
    int32 posY = FROM_FIXED(sensor->position.y);

    int32 solid = 0;
#if RETRO_REV0U
    if (collisionEntity->tileCollisions == TILECOLLISION_DOWN)
        solid = collisionEntity->collisionPlane ? (1 << 15) : (1 << 13);
    else
        solid = collisionEntity->collisionPlane ? (1 << 14) : (1 << 12);
#else
    solid = collisionEntity->collisionPlane ? (1 << 15) : (1 << 13);
#endif

#if RETRO_REV0U
    int32 collideAngle = 0;
    int32 collidePos   = -1;
#endif

    for (int32 l = 0, layerID = 1; l < LAYER_COUNT; ++l, layerID <<= 1) {
        if (collisionEntity->collisionLayers & layerID) {
            TileLayer *layer = &tileLayers[l];
            int32 colX       = posX - layer->position.x;
            int32 colY       = posY - layer->position.y;
            int32 cy         = (colY & -TILE_SIZE) + TILE_SIZE;

            if (colX >= 0 && colX < TILE_SIZE * layer->xsize) {
#if RETRO_REV0U
                int32 stepCount = 2;
#else
                int32 stepCount = 3;
#endif
                for (int32 i = 0; i < stepCount; ++i) {
                    int32 step = -TILE_SIZE;

                    if (cy >= 0 && cy < TILE_SIZE * layer->ysize) {
                        int32 tileX = (colX / TILE_SIZE);
                        int32 tileY = (cy / TILE_SIZE);
                        uint16 tile = layer->layout[tileX + (tileY << layer->widthShift)];

                        if (tile < 0xFFFF && tile & solid) {
                            int32 mask = collisionMasks[collisionEntity->collisionPlane][tile & 0xFFF].roofMasks[colX & 0xF];
#if RETRO_REV0U
                            int32 ty = layer->position.y + cy + mask;
#else
                            int32 ty = cy + mask;
#endif
                            if (mask < 0xFF) {
#if RETRO_REV0U
                                step = TILE_SIZE;
                                if (colY > collidePos) {
                                    collideAngle = tileInfo[collisionEntity->collisionPlane][tile & 0xFFF].roofAngle;
                                    collidePos   = ty;
                                    i            = stepCount;
#else
                                if (colY < ty) {
                                    if (abs(colY - ty) <= collisionMinimumDistance) {
                                        sensor->collided   = true;
                                        sensor->angle      = tileInfo[collisionEntity->collisionPlane][tile & 0xFFF].roofAngle;
                                        sensor->position.y = TO_FIXED(ty + layer->position.y);
                                        i                  = stepCount;
                                    }
#endif
                                }
                            }
                        }
                    }

                    cy += step;
                }
            }

            posX = layer->position.x + colX;
            posY = layer->position.y + colY;
        }
    }

#if RETRO_REV0U
    if (collidePos >= 0 && sensor->position.y <= TO_FIXED(collidePos) && sensor->position.y - TO_FIXED(collidePos) >= -collisionMinimumDistance) {
        sensor->angle      = collideAngle;
        sensor->position.y = TO_FIXED(collidePos);
        sensor->collided   = true;
    }
#endif
}
void RSDK::RWallCollision(CollisionSensor *sensor)
{
    int32 posX = FROM_FIXED(sensor->position.x);
    int32 posY = FROM_FIXED(sensor->position.y);

    int32 solid = collisionEntity->collisionPlane ? (1 << 15) : (1 << 13);

    for (int32 l = 0, layerID = 1; l < LAYER_COUNT; ++l, layerID <<= 1) {
        if (collisionEntity->collisionLayers & layerID) {
            TileLayer *layer = &tileLayers[l];
            int32 colX       = posX - layer->position.x;
            int32 colY       = posY - layer->position.y;
            int32 cx         = (colX & -TILE_SIZE) + TILE_SIZE;

            if (colY >= 0 && colY < TILE_SIZE * layer->ysize) {
                for (int32 i = 0; i < 3; ++i) {
                    if (cx >= 0 && cx < TILE_SIZE * layer->xsize) {
                        uint16 tile = layer->layout[(cx / TILE_SIZE) + ((colY / TILE_SIZE) << layer->widthShift)];

                        if (tile < 0xFFFF && tile & solid) {
                            int32 mask = collisionMasks[collisionEntity->collisionPlane][tile & 0xFFF].rWallMasks[colY & 0xF];
                            int32 tx   = cx + mask;
                            if (mask < 0xFF && colX <= tx && abs(colX - tx) <= 14) {
                                sensor->collided   = true;
                                sensor->angle      = tileInfo[collisionEntity->collisionPlane][tile & 0xFFF].rWallAngle;
                                sensor->position.x = TO_FIXED(tx + layer->position.x);
                                i                  = 3;
                            }
                        }
                    }

                    cx -= TILE_SIZE;
                }
            }

            posX = layer->position.x + colX;
            posY = layer->position.y + colY;
        }
    }
}
