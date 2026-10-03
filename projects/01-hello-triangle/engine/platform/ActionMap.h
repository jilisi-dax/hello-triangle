#pragma once
#include "core/pch.h"

enum class GameAction
{
    MoveForward,
    MoveBackward,
    MoveLeft,
    MoveRight,
    Exit,
    EffectGray,
    EffectInvert,
    EffectSharpen,
    EffectBloomDebug,
    EffectIrradianceDebug,
};

class ActionMap
{
public:
    static void Init();
    static bool IsDown(GameAction a);
    static bool WasPressed(GameAction a);
    static glm::vec2 GetLook();
    static float GetZoom();
    static void RequestExit();

private:
    static std::unordered_map<GameAction, std::vector<int>>& Bindings();
};