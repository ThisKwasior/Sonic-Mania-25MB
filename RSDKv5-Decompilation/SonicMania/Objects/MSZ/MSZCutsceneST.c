// ---------------------------------------------------------------------
// RSDK Project: Sonic Mania
// Object Description: MSZCutsceneST Object
// Object Author: Christian Whitehead/Simon Thomley/Hunter Bridges
// Decompiled by: Rubberduckycooly & RMGRich
// ---------------------------------------------------------------------

#include "Game.h"

ObjectMSZCutsceneST *MSZCutsceneST;

void MSZCutsceneST_Update(void)
{
    RSDK_THIS(MSZCutsceneST);

    if (!self->activated && self->pathNode) {
        MSZCutsceneST->tornadoPath = self->pathNode;
        MSZCutsceneST_SetupCutscene();
        self->activated = true;
    }
}

void MSZCutsceneST_LateUpdate(void) {}

void MSZCutsceneST_StaticUpdate(void)
{
    if (RSDK_GET_ENTITY(SLOT_PAUSEMENU, PauseMenu)->classID == PauseMenu->classID || !MSZCutsceneST->isMayday) {
        if (MSZCutsceneST->playingMaydaySfx) {
            RSDK.StopSfx(MSZCutsceneST->sfxMayday);
            MSZCutsceneST->playingMaydaySfx = false;
        }
    }
    else if (!MSZCutsceneST->playingMaydaySfx) {
        RSDK.PlaySfx(MSZCutsceneST->sfxMayday, true, 255);
        MSZCutsceneST->playingMaydaySfx = true;
    }
}

void MSZCutsceneST_Draw(void) {}

void MSZCutsceneST_Create(void *data)
{
    RSDK_THIS(MSZCutsceneST);

    INIT_ENTITY(self);
    CutsceneRules_SetupEntity(self, &self->size, &self->hitbox);
    self->active = ACTIVE_NEVER;
}

void MSZCutsceneST_StageLoad(void)
{
    MSZCutsceneST->finishedAct = false;

    foreach_all(SignPost, signPost)
    {
        MSZCutsceneST->signPost = signPost;
        foreach_break;
    }

    foreach_all(CutsceneHBH, hbh)
    {
        if (hbh->characterID == HBH_MYSTIC)
            MSZCutsceneST->mystic = hbh;

        if (hbh->characterID == HBH_ROUGE_FANG) {
            MSZCutsceneST->rouges[0]         = hbh;
            MSZCutsceneST->rougePlatforms[0] = RSDK_GET_ENTITY(RSDK.GetEntitySlot(hbh) + 1, Armadiloid);
        }

        if (hbh->characterID == HBH_ROUGE_BEAN) {
            MSZCutsceneST->rouges[1]         = hbh;
            MSZCutsceneST->rougePlatforms[1] = RSDK_GET_ENTITY(RSDK.GetEntitySlot(hbh) + 1, Armadiloid);
        }

        if (hbh->characterID == HBH_ROUGE_BARK) {
            MSZCutsceneST->rouges[2]         = hbh;
            MSZCutsceneST->rougePlatforms[2] = RSDK_GET_ENTITY(RSDK.GetEntitySlot(hbh) + 1, Armadiloid);
        }
    }

    foreach_all(Tornado, tornado)
    {
        MSZCutsceneST->tornado = tornado;
        foreach_break;
    }

    MSZCutsceneST->sfxExplosion3 = RSDK.GetSfx("Stage/Explosion3.wav");
    MSZCutsceneST->sfxMayday     = RSDK.GetSfx("MSZ/Mayday.wav");
    MSZCutsceneST->sfxLedgeBreak = RSDK.GetSfx("Stage/LedgeBreak.wav");

    MSZCutsceneST->active = ACTIVE_ALWAYS;
}

void MSZCutsceneST_SetupCutscene(void)
{
    RSDK_THIS(MSZCutsceneST);

    CutsceneSeq_StartSequence(self, MSZCutsceneST_Cutscene_HandleSignPostLand, MSZCutsceneST_Cutscene_AwaitActFinish,
                              MSZCutsceneST_Cutscene_EnterMystic, MSZCutsceneST_Cutscene_PrepareAmbush, MSZCutsceneST_Cutscene_RougesAmbush,
                              MSZCutsceneST_Cutscene_Mayday, MSZCutsceneST_Cutscene_SetPlayerMSZ2SpawnPos, MSZCutsceneST_Cutscene_PanCameraToPlayer,
                              MSZCutsceneST_Cutscene_SetupMSZ2, StateMachine_None);

#if MANIA_USE_PLUS
    CutsceneSeq_SetSkipType(SKIPTYPE_DISABLED, StateMachine_None);
#endif
}

bool32 MSZCutsceneST_Cutscene_HandleSignPostLand(EntityCutsceneSeq *host)
{
    EntitySignPost *signPost  = MSZCutsceneST->signPost;
    EntityCutsceneHBH *mystic = MSZCutsceneST->mystic;

    if (!host->timer) {
        RSDK.PlaySfx(SignPost->sfxTwinkle, false, 0xFF);
        signPost->state = SignPost_State_Falling;
        mystic->visible = true;
        foreach_all(ParallaxSprite, sprite) { sprite->visible = true; }
    }

    mystic->position.x = signPost->position.x - 0x30000;

    if (signPost->position.y >= MSZCutsceneST->signPostOffsets[mystic->mainAnimator.frameID] + mystic->position.y - 0x3D0000) {
        signPost->position.y = MSZCutsceneST->signPostOffsets[mystic->mainAnimator.frameID] + mystic->position.y - 0x3D0000;
        signPost->state      = SignPost_State_Spin;
        RSDK.PlaySfx(SignPost->sfxSlide, false, 255);
        signPost->spinCount  = 4;
        signPost->velocity.y = 0;
        Music_FadeOut(0.025);
        return true;
    }

    return false;
}

bool32 MSZCutsceneST_Cutscene_AwaitActFinish(EntityCutsceneSeq *host)
{
    MANIA_GET_PLAYER(player1, player2, camera);
    UNUSED(player2);
    UNUSED(camera);

    EntitySignPost *signPost  = MSZCutsceneST->signPost;
    EntityCutsceneHBH *mystic = MSZCutsceneST->mystic;

    signPost->position.y = MSZCutsceneST->signPostOffsets[mystic->mainAnimator.frameID] + mystic->position.y - 0x3D0000;
    if (signPost->state == SignPost_State_Done) {
        player1->stateInput = StateMachine_None;
        CutsceneSeq_LockAllPlayerControl();
        player1->right     = false;
        player1->left      = false;
        player1->down      = false;
        player1->up        = false;
        player1->jumpPress = false;
        player1->jumpHold  = false;
    }

    if (MSZCutsceneST->finishedAct) {
        mystic->oscillate = false;

        TileLayer *fgLow = RSDK.GetTileLayer(Zone->fgLayer[0]);
        for (int32 i = 0; i < fgLow->scrollInfoCount; ++i) {
            fgLow->scrollInfo[i].parallaxFactor = 0x100;
            fgLow->scrollInfo[i].scrollSpeed    = 0;
        }

#if MANIA_USE_PLUS
        if (RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq)->classID)
            RSDK_GET_ENTITY(SLOT_CUTSCENESEQ, CutsceneSeq)->skipType = SKIPTYPE_NEXTSCENE;
#endif
        return true;
    }
    return false;
}

bool32 MSZCutsceneST_Cutscene_EnterMystic(EntityCutsceneSeq *host)
{
    MANIA_GET_PLAYER(player1, player2, camera);
    UNUSED(player2);
    UNUSED(camera);

    EntityTornado *tornado    = MSZCutsceneST->tornado;
    EntitySignPost *signPost  = MSZCutsceneST->signPost;
    EntityCutsceneHBH *mystic = MSZCutsceneST->mystic;

    if (!host->timer) {
        Music_PlayTrack(TRACK_HBHMISCHIEF);
        RSDK.SetSpriteAnimation(player1->aniFrames, ANI_RIDE, &player1->animator, false, 0);
        player1->direction = FLIP_NONE;
    }

    if (!host->values[0]) {
        int32 x = (tornado->position.x & 0xFFFF0000) + 0x200000;
        if (player1->position.x != x)
            player1->position.x += (x - player1->position.x) >> 1;

        if (abs(player1->position.x - x) < 0x10000) {
            player1->position.x            = x;
            MSZCutsceneST->tornadoStartPos = tornado->position;
            host->storedTimer              = host->timer;
            host->values[0]                = true;
        }
    }

    tornado->drawGroup = Zone->playerDrawGroup[0];
    player1->drawGroup = Zone->playerDrawGroup[1];
    mystic->velocity.y -= 0x800;
    mystic->position.y += mystic->velocity.y;
    mystic->drawGroup    = Zone->objectDrawGroup[0];
    signPost->position.y = mystic->position.y - 0x3D0000 + MSZCutsceneST->signPostOffsets[mystic->mainAnimator.frameID];

    Vector2 range;
    range.x = ScreenInfo->size.x << 16;
    range.y = ScreenInfo->size.y << 16;
    if (!RSDK.CheckOnScreen(mystic, &range)) {
        host->values[2] = true;
        destroyEntity(mystic);
        return true;
    }
    return false;
}

bool32 MSZCutsceneST_Cutscene_PrepareAmbush(EntityCutsceneSeq *host)
{
    MANIA_GET_PLAYER(player1, player2, camera);
    UNUSED(player2);
    UNUSED(camera);

    EntityTornado *tornado      = MSZCutsceneST->tornado;
    EntityTornadoPath *pathNode = MSZCutsceneST->tornadoPath;
    int32 x                     = pathNode->position.x - 0x600000;
    int32 y                     = pathNode->position.y + 0x300000;

    if (host->timer - host->storedTimer == 120 && !host->values[1]) {
        tornado->position.x = x;
        tornado->position.y = y;
        host->values[0]     = 1;
        return true;
    }
    else {
        MathHelpers_LerpSin512(&tornado->position, 255 * (host->timer - host->storedTimer) / 120, MSZCutsceneST->tornadoStartPos.x,
                               MSZCutsceneST->tornadoStartPos.y, x, y);
        tornado->position.x &= 0xFFFF0000;
        tornado->position.y &= 0xFFFF0000;

        player1->position.x = (tornado->position.x & 0xFFFF0000) + 0x200000;
        if (player1->characterID == ID_TAILS)
            player1->position.y = (tornado->position.y & 0xFFFF0000) - 0x2C0000;
        else
            player1->position.y = (tornado->position.y & 0xFFFF0000) - 0x300000;
    }

    return false;
}

bool32 MSZCutsceneST_Cutscene_RougesAmbush(EntityCutsceneSeq *host)
{
    bool32 (*rougeStates[3])(EntityCutsceneSeq *);
    rougeStates[0] = MSZCutsceneST_Cutscene_ShowFang;
    rougeStates[1] = MSZCutsceneST_Cutscene_ShowBean;
    rougeStates[2] = MSZCutsceneST_Cutscene_ShowBark;

    if (!host->timer) {
        MSZCutsceneST->rougeID = RSDK.Rand(0, 8) % 3;

        for (int32 i = 0; i < 3; ++i) {
            if (i != MSZCutsceneST->rougeID) {
                destroyEntity(MSZCutsceneST->rouges[i]);
                destroyEntity(MSZCutsceneST->rougePlatforms[i]);
            }
        }
    }

    EntityArmadiloid *armadiloid      = MSZCutsceneST->rougePlatforms[MSZCutsceneST->rougeID];
    armadiloid->drawGroup             = Zone->objectDrawGroup[1];
    armadiloid->boosterAnimator.speed = 1;
    armadiloid->velocity.x            = 0;

    return rougeStates[MSZCutsceneST->rougeID](host);
}

bool32 MSZCutsceneST_Cutscene_ShowFang(EntityCutsceneSeq *host)
{
    EntityTornado *tornado       = MSZCutsceneST->tornado;
    EntityTornadoPath *pathNode  = MSZCutsceneST->tornadoPath;
    EntityCutsceneHBH *rouge     = MSZCutsceneST->rouges[MSZCutsceneST->rougeID];
    EntityArmadiloid *armadiloid = MSZCutsceneST->rougePlatforms[MSZCutsceneST->rougeID];

    if (!host->timer)
        armadiloid->state = Armadiloid_State_PlatformFlying;

    int32 x = pathNode->position.x + 0x600000;
    int32 y = armadiloid->startPos.y;

    if (armadiloid->position.x > x) {
        MathHelpers_Lerp2Sin1024(&armadiloid->position, 255 * host->timer / 180, armadiloid->startPos.x, armadiloid->startPos.y, x, y);
        armadiloid->position.x &= 0xFFFF0000;
        armadiloid->position.y &= 0xFFFF0000;
    }

    if (host->timer == 180) {
        armadiloid->position.x = x;
        armadiloid->position.y = y;
        host->values[0]        = 1;
    }

    if (host->values[0] && !host->values[1]) {
        if (!rouge->mainAnimator.animationID)
            RSDK.SetSpriteAnimation(rouge->aniFrames, 1, &rouge->mainAnimator, true, 0);

        if (rouge->mainAnimator.animationID == 1 && rouge->mainAnimator.frameID == rouge->mainAnimator.frameCount - 1
            && rouge->mainAnimator.timer == 12) {
            RSDK.SetSpriteAnimation(rouge->aniFrames, 2, &rouge->mainAnimator, true, 0);
            rouge->velocity.y = -0x20000;
            RSDK.PlaySfx(HeavyMystic->sfxPon, false, 255);

            EntityDebris *debris = CREATE_ENTITY(Debris, NULL, rouge->position.x, rouge->position.y - 0x40000);
            debris->position.x -= 0x180000;
            debris->velocity.x = -0x20000;
            debris->drawGroup  = Zone->playerDrawGroup[1];
            debris->state      = Debris_State_Move;
            debris->direction  = FLIP_X;
            debris->drawFX     = FX_FLIP;
            RSDK.SetSpriteAnimation(rouge->aniFrames, 4, &debris->animator, true, 0);

            MSZCutsceneST->projectile = (Entity *)debris;
        }

        if (rouge->mainAnimator.animationID == 2) {
            rouge->position.y += rouge->velocity.y;
            rouge->velocity.y += 0x3800;
        }
    }

    rouge->position.x = armadiloid->position.x + 0x20000;
    if (rouge->position.y > armadiloid->position.y - 0x200000) {
        rouge->position.y = armadiloid->position.y - 0x200000;
        rouge->velocity.y = 0;

        if (rouge->mainAnimator.animationID == 2) {
            RSDK.SetSpriteAnimation(rouge->aniFrames, 1, &rouge->mainAnimator, true, 2);
            rouge->mainAnimator.loopIndex = 3;
            host->values[1]               = 1;
        }
    }

    if (MSZCutsceneST->projectile && MSZCutsceneST->projectile->position.x <= tornado->position.x + 0x300000) {
        destroyEntity(MSZCutsceneST->projectile);
        return true;
    }

    return false;
}

bool32 MSZCutsceneST_Cutscene_ShowBean(EntityCutsceneSeq *host)
{
    EntityTornado *tornado       = MSZCutsceneST->tornado;
    EntityTornadoPath *pathNode  = MSZCutsceneST->tornadoPath;
    EntityCutsceneHBH *rouge     = MSZCutsceneST->rouges[MSZCutsceneST->rougeID];
    EntityArmadiloid *armadiloid = MSZCutsceneST->rougePlatforms[MSZCutsceneST->rougeID];

    int32 x = pathNode->position.x + 0x600000;
    int32 y = armadiloid->startPos.y;

    if (!host->timer)
        armadiloid->state = Armadiloid_State_PlatformFlying;

    if (armadiloid->position.x > x) {
        MathHelpers_Lerp2Sin1024(&armadiloid->position, 255 * host->timer / 180, armadiloid->startPos.x, armadiloid->startPos.y, x, y);
        armadiloid->position.x &= 0xFFFF0000;
        armadiloid->position.y &= 0xFFFF0000;
    }

    if (host->timer == 180) {
        armadiloid->position.x = x;
        armadiloid->position.y = y;
        host->values[0]        = 1;
    }

    if (host->values[0] && !host->values[1]) {
        if (rouge->mainAnimator.animationID == 5) {
            RSDK.SetSpriteAnimation(rouge->aniFrames, 7, &rouge->mainAnimator, true, 0);
        }

        if (rouge->mainAnimator.animationID == 7) {
            if (rouge->mainAnimator.frameID == 2 && !MSZCutsceneST->projectile) {
                RSDK.PlaySfx(HeavyMystic->sfxDrop, false, 255);
                EntityDebris *debris = CREATE_ENTITY(Debris, NULL, rouge->position.x, rouge->position.y - 0x130000);
                debris->position.x += 0xB0000;
                debris->velocity.x      = -0x48000;
                debris->velocity.y      = -0x40000;
                debris->gravityStrength = 0x3800;
                debris->drawGroup       = Zone->playerDrawGroup[1];
                debris->state           = Debris_State_Fall;
                RSDK.SetSpriteAnimation(rouge->aniFrames, 11, &debris->animator, true, 0);
                MSZCutsceneST->projectile = (Entity *)debris;
            }

            if (rouge->mainAnimator.animationID == 7 && rouge->mainAnimator.frameID == 5 && rouge->mainAnimator.timer == 2) {
                RSDK.SetSpriteAnimation(rouge->aniFrames, 5, &rouge->mainAnimator, true, 0);
                host->values[1] = 1;
            }
        }
    }

    rouge->position.x = armadiloid->position.x + 0x40000;
    rouge->position.y = armadiloid->position.y - 0x200000;

    if (MSZCutsceneST->projectile && MSZCutsceneST->projectile->position.y >= tornado->position.y - 0x100000) {
        destroyEntity(MSZCutsceneST->projectile);
        return true;
    }

    return false;
}

bool32 MSZCutsceneST_Cutscene_ShowBark(EntityCutsceneSeq *host)
{
    EntityTornado *tornado       = MSZCutsceneST->tornado;
    EntityTornadoPath *pathNode  = MSZCutsceneST->tornadoPath;
    EntityCutsceneHBH *rouge     = MSZCutsceneST->rouges[MSZCutsceneST->rougeID];
    EntityArmadiloid *armadiloid = MSZCutsceneST->rougePlatforms[MSZCutsceneST->rougeID];

    int32 x = pathNode->position.x + 0x600000;
    int32 y = armadiloid->startPos.y;

    if (!host->timer)
        armadiloid->state = Armadiloid_State_PlatformFlying;

    if (armadiloid->position.x > x) {
        MathHelpers_Lerp2Sin1024(&armadiloid->position, 255 * host->timer / 180, armadiloid->startPos.x, armadiloid->startPos.y, x, y);
        armadiloid->position.x &= 0xFFFF0000;
        armadiloid->position.y &= 0xFFFF0000;
    }

    if (host->timer == 180) {
        armadiloid->position.x = x;
        armadiloid->position.y = y;
        host->values[0]        = 1;
    }

    if (host->values[0] && !host->values[1]) {
        if (rouge->mainAnimator.animationID == 12)
            RSDK.SetSpriteAnimation(rouge->aniFrames, 14, &rouge->mainAnimator, true, 0);

        if (rouge->mainAnimator.animationID == 14) {
            if (rouge->mainAnimator.frameID == 3 && rouge->mainAnimator.timer == 1) {
                RSDK.PlaySfx(HeavyMystic->sfxImpact2, false, 255);
                Camera_ShakeScreen(0, 0, 2);
                armadiloid->timer = 0;
                armadiloid->state = Armadiloid_State_PlatformShoot;
                RSDK.SetSpriteAnimation(Armadiloid->aniFrames, 2, &armadiloid->headAnimator, true, 0);
            }

            if (rouge->mainAnimator.animationID == 14 && rouge->mainAnimator.frameID == 7 && rouge->mainAnimator.timer == 2) {
                RSDK.SetSpriteAnimation(rouge->aniFrames, 12, &rouge->mainAnimator, true, 0);
                host->values[1] = true;
            }
        }
    }

    if (!MSZCutsceneST->projectile) {
        foreach_active(Projectile, projectile)
        {
            MSZCutsceneST->projectile = (Entity *)projectile;
            projectile->drawGroup     = Zone->playerDrawGroup[1];
            foreach_break;
        }
    }

    rouge->position.x = armadiloid->position.x + 0x40000;
    rouge->position.y = armadiloid->position.y - 0x2C0000;

    if (MSZCutsceneST->projectile && MSZCutsceneST->projectile->position.x <= tornado->position.x + 0x300000) {
        destroyEntity(MSZCutsceneST->projectile);
        return true;
    }

    return false;
}

bool32 MSZCutsceneST_Cutscene_Mayday(EntityCutsceneSeq *host)
{
    MANIA_GET_PLAYER(player1, player2, camera);
    UNUSED(camera);

    EntityTornado *tornado      = MSZCutsceneST->tornado;
    EntityTornadoPath *pathNode = MSZCutsceneST->tornadoPath;

    if (!host->timer) {
        RSDK.PlaySfx(MSZCutsceneST->sfxExplosion3, false, 255);
        host->fadeWhite         = 512;
        MSZCutsceneST->isMayday = true;
        RSDK.SetSpriteAnimation(-1, 0, &tornado->animatorPropeller, true, 0);
        RSDK.SetSpriteAnimation(-1, 0, &tornado->animatorPilot, true, 0);
        tornado->drawFX |= FX_ROTATE;
        tornado->state     = Tornado_State_Mayday;
        tornado->rotation  = 48;
        player1->state     = Player_State_Static;
        player1->direction = FLIP_X;
        RSDK.SetSpriteAnimation(player1->aniFrames, ANI_CLING, &player1->animator, true, 0);
        player1->rotation = tornado->rotation;
        tornado->active   = ACTIVE_NORMAL;
        pathNode->state   = TornadoPath_State_SetTornadoSpeed;
        destroyEntity(MSZCutsceneST->rouges[MSZCutsceneST->rougeID]);
        destroyEntity(MSZCutsceneST->rougePlatforms[MSZCutsceneST->rougeID]);

        TileLayer *background1 = RSDK.GetTileLayer(0);
        for (int32 i = 0; i < background1->scrollInfoCount; ++i) background1->scrollInfo[i].scrollPos = 0;

        TileLayer *background2 = RSDK.GetTileLayer(1);
        for (int32 i = 0; i < background2->scrollInfoCount; ++i) background2->scrollInfo[i].scrollPos = 0;

        foreach_active(ParallaxSprite, sprite) { sprite->scrollPos.x = 0; }

        MSZSetup_ReloadBGParallax_Multiply(0x800);
    }

    if (!host->values[2]) {
        player1->position.x = tornado->position.x - 0x280000;
        if (player1->characterID == ID_TAILS)
            player1->position.y = tornado->position.y - 0x340000;
        else
            player1->position.y = tornado->position.y - 0x380000;
    }

    if (!host->values[0]) {
        host->fadeWhite = CLAMP(512 - ((host->timer - 45) << 9) / 60, 0, 512);
        if (!host->fadeWhite)
            host->values[0] = 1;
    }

    if (host->timer == 150) {
        player1->onGround        = true;
        player1->velocity.x      = 0;
        player1->velocity.y      = 0;
        player1->groundVel       = 0;
        player1->nextAirState    = StateMachine_None;
        player1->nextGroundState = StateMachine_None;
        Player_Action_Jump(player1);
        player1->direction  = FLIP_NONE;
        player1->velocity.x = 0xE0000;
        host->values[2]     = true;
        if (player2->classID == Player->classID) {
            player2->state  = MSZSetup_PlayerState_PostCrashJumpIn;
            player2->active = ACTIVE_NORMAL;
        }
    }

    if (TornadoPath->moveVel.y > 0 && !(Zone->timer % 5))
        Camera_ShakeScreen(0, 2, 2);

    if (TornadoPath->moveVel.y < 0 && !host->values[3]) {
        MSZCutsceneST->isMayday = false;
        Camera_ShakeScreen(0, 0, 5);
        MSZSetup_ReloadBGParallax_Multiply(0x200);
        host->values[3]   = true;
        tornado->onGround = true;
        host->storedTimer = host->timer;
        Music_FadeOut(0.02);
        RSDK.PlaySfx(MSZCutsceneST->sfxLedgeBreak, false, 255);
    }

    if (tornado->onGround && !((host->timer - host->storedTimer) % 16)) {
        EntityDebris *debris = CREATE_ENTITY(Debris, NULL, tornado->position.x, tornado->position.y);
        debris->position.x += 0x2C0000;
        debris->position.y += 0xC0000;
        debris->drawGroup = Zone->objectDrawGroup[0];
        debris->state     = Debris_State_Move;
        debris->timer     = 53;
        RSDK.SetSpriteAnimation(UberCaterkiller->aniFrames, 4, &debris->animator, true, 0);
        MSZCutsceneST->projectile = (Entity *)debris;
    }

    if (TornadoPath->crashFinished) {
        MSZSetup_ReloadBGParallax_Multiply(0x000);
        tornado->state = StateMachine_None;
        return true;
    }

    return false;
}

bool32 MSZCutsceneST_Cutscene_SetPlayerMSZ2SpawnPos(EntityCutsceneSeq *host)
{
    EntityPlayer *player1 = RSDK_GET_ENTITY(SLOT_PLAYER1, Player);

    if (!host->timer) {
        player1->position.x = 0x3DA00000;
        player1->position.y = 0x5300000;
    }

    return host->timer == 90;
}

bool32 MSZCutsceneST_Cutscene_PanCameraToPlayer(EntityCutsceneSeq *host)
{
    MANIA_GET_PLAYER(player1, player2, camera);

    if (!host->timer) {
        TornadoPath->camera = NULL;
        camera->boundsB     = 0x5A00000;
        Camera_SetupLerp(CAMERA_LERP_NORMAL, 0, player1->position.x + 0x340000, player1->position.y - 0x140000, 2);
        MSZSetup_ReloadBGParallax_Multiply(0x400);
    }

    if (camera->position.x == player1->position.x + 0x340000 && camera->position.y == player1->position.y - 0x140000) {
        MSZSetup_ReloadBGParallax_Multiply(0x000);

        TileLayer *background1 = RSDK.GetTileLayer(0);
        for (int32 i = 0; i < background1->scrollInfoCount; ++i) background1->scrollInfo[i].scrollPos &= 0xFFFF0000;

        TileLayer *background2 = RSDK.GetTileLayer(1);
        for (int32 i = 0; i < background2->scrollInfoCount; ++i) background2->scrollInfo[i].scrollPos &= 0xFFFF0000;

        foreach_active(ParallaxSprite, sprite) { sprite->scrollPos.x &= 0xFFFF0000; }

        Zone->cameraBoundsL[0] = (player1->position.x >> 16) - 160;
        Zone->cameraBoundsB[0] = (player1->position.y >> 16) - 112;
        if (player2->classID != Player->classID || player2->onGround)
            return true;
    }

    return false;
}

bool32 MSZCutsceneST_Cutscene_SetupMSZ2(EntityCutsceneSeq *host)
{
    if (host->timer == 10) {
        MSZSetup_State_StoreMSZ1ScrollPos_ST();
        return true;
    }

    return false;
}

#if RETRO_INCLUDE_EDITOR
void MSZCutsceneST_EditorDraw(void)
{
    RSDK_THIS(MSZCutsceneST);

    CutsceneRules_DrawCutsceneBounds(self, &self->size);
}

void MSZCutsceneST_EditorLoad(void) {}
#endif

void MSZCutsceneST_Serialize(void) { RSDK_EDITABLE_VAR(MSZCutsceneST, VAR_VECTOR2, size); }
