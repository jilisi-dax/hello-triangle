#include "ActionMap.h"
#include "Input.h"

std::unordered_map<GameAction, std::vector<int>>& ActionMap::Bindings()
{
    static std::unordered_map<GameAction, std::vector<int>> b;
    return b;
}

void ActionMap::Init()
{
    auto& b = Bindings();
    b[GameAction::MoveForward] = { GLFW_KEY_W, GLFW_KEY_UP }; 
    b[GameAction::MoveBackward] = { GLFW_KEY_S, GLFW_KEY_DOWN };
    b[GameAction::MoveLeft] = { GLFW_KEY_A, GLFW_KEY_LEFT };
    b[GameAction::MoveRight] = { GLFW_KEY_D, GLFW_KEY_RIGHT };
    b[GameAction::Exit] = { GLFW_KEY_ESCAPE };
}

bool ActionMap::IsDown(GameAction a)
{
    auto it = Bindings().find(a);
    if (it == Bindings().end()) return false;
    for (int key : it->second)
        if (Input::IsDown(key)) return true;
    return false;
}

bool ActionMap::WasPressed(GameAction a)
{
    auto it = Bindings().find(a);
    if (it == Bindings().end()) return false;
    for (int key : it->second)
        if (Input::WasPressed(key)) return true;
    return false;
}

glm::vec2 ActionMap::GetLook() { return Input::GetMouseDelta(); }
float    ActionMap::GetZoom() { return Input::GetScrollDelta(); }
void     ActionMap::RequestExit() { Input::RequestExit(); }

