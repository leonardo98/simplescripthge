#ifndef DIALOGS_H
#define DIALOGS_H

HWND InitDialogs(HINSTANCE hInstance);

void CloseDialogs();

bool SetDialogsOnTop();

void SetBonePropEnabled(bool enable);

void Draw();

void Update(float dt);

void CreateAnimationTree();

bool Exit(bool exit = false);

#endif//DIALOGS_H