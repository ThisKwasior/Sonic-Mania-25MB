#include "RSDK/Core/RetroEngine.hpp"

// ====================
// API Cores
// ====================

namespace RSDK
{
namespace SKU
{
// Dummy API
#if RETRO_USERCORE_DUMMY
#include "RSDK/User/Dummy/DummyAchievements.cpp"
#endif

// Steam API
#if RETRO_USERCORE_STEAM
#include "RSDK/User/Steam/SteamAchievements.cpp"
#endif

// Epic Games API
#if RETRO_USERCORE_EOS
#include "RSDK/User/EOS/EOSAchievements.cpp"
#endif

// Switch API
#if RETRO_USERCORE_NX
#include "RSDK/User/NX/NXAchievements.cpp"
#endif

} // namespace SKU
} // namespace RSDK

using namespace RSDK;

#if RETRO_REV02
SKU::UserAchievements *RSDK::SKU::achievements = NULL;
#endif

void RSDK::SKU::ClearAchievements() { PrintLog(PRINT_NORMAL, "DUMMY ClearAchievements()"); }

#if !RETRO_REV02
void RSDK::SKU::TryUnlockAchievement(AchievementID *id)
{
    PrintLog(PRINT_NORMAL, "DUMMY TryUnlockAchievement(%s)", id->identifier);

    int32 i = 0;
    for (; i < (int32)achievementList.size(); ++i) {
        if (achievementList[i].identifier == id->identifier) {
            if (!achievementList[i].achieved) {
                achievementStack.push_back(i);
                PrintLog(PRINT_NORMAL, "Unlocked Achievement: (%s, %d)", id->identifier, i);
                achievementList[i].achieved = true;
                SaveUserData();
            }
            break;
        }
    }

    if (i == achievementList.size())
        PrintLog(PRINT_NORMAL, "Failed to Unlock Achievement: (%s)", id->identifier);
}
#endif

#if RETRO_VER_EGS || RETRO_USE_DUMMY_ACHIEVEMENTS
bool32 RSDK::SKU::achievementsEnabled     = true;
uint16 RSDK::SKU::achievementAniFrames[2] = { 0, 0 };
Animator RSDK::SKU::achievementAnimator[2];
String RSDK::SKU::achievementStrings[2];
int32 RSDK::SKU::achievementStringWidth[2]  = { 0, 0 };
int32 RSDK::SKU::achievementID              = 0;
int32 RSDK::SKU::achievementDisplayDuration = 0;
int32 RSDK::SKU::achievementsDrawn          = 0;
int32 RSDK::SKU::achievementStrW            = 0;
int32 RSDK::SKU::achievementStrX            = 0;
bool32 RSDK::SKU::achievementsLoaded        = false;
bool32 RSDK::SKU::achievementDrawReady      = false;
bool32 RSDK::SKU::achievementForceReset     = false;

void RSDK::SKU::LoadAchievementAssets()
{
    if (achievements && achievementsEnabled && achievements->CheckAchievementsEnabled()) {
        if (achievements->CheckAchievementPopupEnabled()) {
            if (achievementForceReset) {
                achievementForceReset      = false;
                achievementID              = 0;
                achievementDrawReady       = false;
                achievementDisplayDuration = 0;
                achievementsDrawn          = false;
            }
            else {
                if (achievementID)
                    achievementDisplayDuration = 3 * 60; // 3 seconds
            }

            achievementsLoaded = !CheckSceneFolder("Logos") && !CheckSceneFolder("Title");

            if (achievementsLoaded) {
                achievementAniFrames[0] = LoadSpriteAnimation("UI/SmallFont.bin", SCOPE_STAGE);
                SetSpriteAnimation(achievementAniFrames[0], 0, &achievementAnimator[0], true, 0);

                achievementAniFrames[1] = LoadSpriteAnimation("UI/SmallFont.bin", SCOPE_STAGE);
                SetSpriteAnimation(achievementAniFrames[1], 0, &achievementAnimator[1], true, 0);
            }
        }
        else {
            achievementsLoaded = false;
        }
    }
}
void RSDK::SKU::ProcessAchievements()
{
    if (achievementsEnabled && achievements && achievements->CheckAchievementsEnabled()) {
        if (achievementsLoaded) {
            if (!achievementID) {
                achievementID = achievements->GetNextAchievementID();
                if (achievementID) {
                    achievementDrawReady       = true;
                    achievementDisplayDuration = 3 * 60; // 3 seconds
                    achievementsDrawn          = false;
                    achievements->RemoveLastAchievementID();

                    InitString(&achievementStrings[0], achievementText.c_str(), 0);
                    String buffer;
                    CopyString(&achievementStrings[1], achievements->GetAchievementName(&buffer, achievementID));
#if !RETRO_USE_ORIGINAL_CODE
                    RemoveStorageEntry((void**)&buffer);
#endif

                    if (curSKU.language == LANGUAGE_JP) {
                        achievementStringWidth[0] = 13 * achievementStrings[0].length;
                        achievementStringWidth[1] = 13 * achievementStrings[1].length;
                    }
                    else {
                        achievementStringWidth[0] =
                            GetStringWidth(achievementAniFrames[0], 0, &achievementStrings[0], 0, achievementStrings[0].length, 0);
                        achievementStringWidth[1] =
                            GetStringWidth(achievementAniFrames[1], 0, &achievementStrings[1], 0, achievementStrings[1].length, 0);
                    }

                    achievementStrW = MAX(achievementStringWidth[0], achievementStringWidth[1]) + 16;
                    achievementStrX = achievementStringWidth[1] > achievementStringWidth[0] ? 20 : 0;
                }
            }

            if (achievementsDrawn) {
                if (!--achievementDisplayDuration) {
                    achievementID        = 0;
                    achievementDrawReady = false;
                }
            }
        }
    }
}
void RSDK::SKU::DrawAchievements()
{
    if (achievementsEnabled && achievements && achievements->CheckAchievementsEnabled()) {
        if (achievementsLoaded && achievementDrawReady && achievementID) {
            Vector2 drawPos;

            InitString(&achievementStrings[0], achievementText.c_str(), 0);
            String buffer;
            CopyString(&achievementStrings[1], achievements->GetAchievementName(&buffer, achievementID));
#if !RETRO_USE_ORIGINAL_CODE
            RemoveStorageEntry((void**)&buffer);
#endif

            int32 drawX = achievementStrX + currentScreen->size.x - achievementStrW;
            DrawRectangle(drawX, currentScreen->size.y - 40, achievementStrW - achievementStrX, 40, 0xFF107C, 0x10, INK_NONE, true);

            Vector2 vertices[3];
            vertices[0].x = TO_FIXED(drawX - 40);
            vertices[0].y = TO_FIXED(currentScreen->size.y);
            vertices[1].x = TO_FIXED(drawX);
            vertices[1].y = TO_FIXED(currentScreen->size.y - 40);
            vertices[2].x = TO_FIXED(drawX);
            vertices[2].y = TO_FIXED(currentScreen->size.y);
            DrawFace(vertices, 3, 0xFF, 0x10, 0x7C, 0x10, INK_NONE);

            drawPos.x = TO_FIXED(drawX - achievementStrX + achievementStrW - 8);
            drawPos.y = vertices[1].y + TO_FIXED(10);
            SetSpriteString(achievementAniFrames[0], 0, &achievementStrings[0]);
            DrawString(&achievementAnimator[0], &drawPos, &achievementStrings[0], 0, achievementStrings[0].length, ALIGN_CENTER, 0, 0, NULL, true);

            vertices[1].y += TO_FIXED(28);

            drawPos.x = TO_FIXED(drawX - achievementStrX + achievementStrW - 8);
            drawPos.y = vertices[1].y;
            SetSpriteString(achievementAniFrames[1], 0, &achievementStrings[1]);
            DrawString(&achievementAnimator[1], &drawPos, &achievementStrings[1], 0, achievementStrings[1].length, ALIGN_CENTER, 0, 0, NULL, true);

            achievementsDrawn = true;
        }
    }
}

#endif

// Start custom achievement code

// Start Dummy Achievements
std::string RSDK::achievementText = "Achievement!";
std::vector<RSDK::AchievementInfo> RSDK::achievementList;
std::vector<int32> RSDK::achievementStack;
// End Dummy Achievements
//
// this is added because we don't have access to any store APIs that would otherwise use this feature
void RSDK::RegisterAchievement(const char *identifier, const char *name, const char *desc)
{
    AchievementInfo info;
    info.identifier  = identifier;
    info.name        = name;
    info.description = desc;
    info.achieved    = false;
    achievementList.push_back(info);
}

// End custom achievement code