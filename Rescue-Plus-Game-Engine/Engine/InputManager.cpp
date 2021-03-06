#include "InputManager.h"

//Initialize default values
void InputManager::Init(HWND hWnd)
{
	this->hWnd = hWnd;

	winRequireFocus = true;
	mb_L_Down = false;
	mb_R_Down = false;
	mb_M_Down = false;
	prev_MB_L_Down = false;
	prev_MB_R_Down = false;
	prev_MB_M_Down = false;
	wheelDelta = 0;

	GetCursorPos(&mousePos);
	prevMousePos = mousePos;
}

// Deinitialize values
void InputManager::Release()
{

}

//Update the focus state of the window
void InputManager::UpdateFocus()
{
	if (GetFocus() == hWnd)
	{
		windowFocused = true;
	}
	else windowFocused = false;
}

//Update the input manager (only call ONCE PER FRAME!)
void InputManager::UpdateStates()
{
	//Previous mouse button states
	prev_MB_L_Down = mb_L_Down;
	prev_MB_R_Down = mb_R_Down;
	prev_MB_M_Down = mb_M_Down;
	wheelDelta = 0;

	prevKeyboardState = keyboardState;
	for (int i = 0; i < 256; i++) {
		keyboardState[i] = GetAsyncKeyState(i);
	}
}

//Update the mouse position (only call ONCE PER FRAME!)
void InputManager::UpdateMousePos()
{
	//Get new cursor position
	prevMousePos = mousePos;
	GetCursorPos(&mousePos);
}

// Change the window focus requirement
void InputManager::SetWindowFocusRequirement(bool windowMustBeFocused)
{
	winRequireFocus = windowMustBeFocused;
}

// Get the focus of the window
// If the focus requirement is false, then this will always return true
bool InputManager::IsWindowFocused()
{
	if (winRequireFocus)
		return windowFocused;
	return true;
}

// Caputure the mouse so we keep getting mouse move events even if the
// mouse leaves the window
void InputManager::CaptureWindow()
{
	SetCapture(hWnd);
}

// Release the control of the mouse
void InputManager::ReleaseWindow()
{
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void InputManager::OnMouseDown(WPARAM buttonState, int x, int y)
{
	//Find what button was pressed
	if (buttonState & 0x0001) { mb_L_Down = true; }
	else if (buttonState & 0x0002) { mb_R_Down = true; }
	else if (buttonState & 0x0010) { mb_M_Down = true; }
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void InputManager::OnMouseUp(WPARAM buttonState, int x, int y, int button)
{
	//Find what button was released
	if (button & 0x0001) { mb_L_Down = false; }
	else if (button & 0x0002) { mb_R_Down = false; }
	else if (button & 0x0010) { mb_M_Down = false; }
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void InputManager::OnMouseWheel(float wheelDelta, int x, int y)
{
	//Store wheel delta
	this->wheelDelta = wheelDelta;
}

//Returns true while the inputted key is held down
bool InputManager::GetKey(Key key)
{
	//Early return if we need focus and we don't have it
	if (winRequireFocus && !windowFocused)
		return false;

	return (keyboardState[(int)key] & 0x8000) != 0;
}

//Returns true during the frame the user pressed down the inputted key
bool InputManager::GetKeyDown(Key key)
{
	//Early return if we need focus and we don't have it
	if (winRequireFocus && !windowFocused)
		return false;

	return ((keyboardState[(int)key] & 0x8000) != 0)
		&& ((prevKeyboardState[(int)key] & 0x8000) == 0);
}

//Returns true the first frame the user releases the inputted key
bool InputManager::GetKeyUp(Key key)
{
	//Early return if we need focus and we don't have it
	if (winRequireFocus && !windowFocused)
		return false;

	return ((keyboardState[(int)key] & 0x8000) == 0)
		&& ((prevKeyboardState[(int)key] & 0x8000) != 0);
}

// Returns true while the inputted mouse button is held down
bool InputManager::GetMouseButton(MouseButtons button)
{
	//Early return if we need focus and we don't have it
	if (winRequireFocus && !windowFocused)
		return false;

	//Find the correct case for the input
	switch (button)
	{
		case MouseButtons::L:
			return mb_L_Down && prev_MB_L_Down;
			break;

		case MouseButtons::R:
			return mb_R_Down && prev_MB_R_Down;
			break;

		case MouseButtons::M:
			return mb_M_Down && prev_MB_M_Down;
			break;

		default:
			return false;
			break;
	}
}

// Returns true during the frame the user pressed down the mouse button
bool InputManager::GetMouseButtonDown(MouseButtons button)
{
	//Early return if we need focus and we don't have it
	if (winRequireFocus && !windowFocused)
		return false;

	//Find the correct case for the input
	switch (button)
	{
		case MouseButtons::L:
			return mb_L_Down && !prev_MB_L_Down;
			break;

		case MouseButtons::R:
			return mb_R_Down && !prev_MB_R_Down;
			break;

		case MouseButtons::M:
			return mb_M_Down && !prev_MB_M_Down;
			break;

		default:
			return false;
			break;
	}
}

//Returns true the first frame the user releases the mouse button
bool InputManager::GetMouseButtonUp(MouseButtons button)
{
	//Early return if we need focus and we don't have it
	if (winRequireFocus && !windowFocused)
		return false;

	//Find the correct case for the input
	switch (button)
	{
		case MouseButtons::L:
			return !mb_L_Down && !prev_MB_L_Down;
			break;

		case MouseButtons::R:
			return !mb_R_Down && !prev_MB_R_Down;
			break;

		case MouseButtons::M:
			return !mb_M_Down && !prev_MB_M_Down;
			break;

		default:
			return false;
			break;
	}
}

// If the mouse moved between this frame and the last
bool InputManager::DidMouseMove()
{
	return (mousePos.x != prevMousePos.x) && (mousePos.y != prevMousePos.y);
}

// Get the current X mouse position
long InputManager::GetMouseX()
{
	return mousePos.x;
}
// Get the current Y mouse position
long InputManager::GetMouseY()
{
	return mousePos.y;
}

// Get the previous frame's X mouse position
long InputManager::GetPreviousMouseX()
{
	return prevMousePos.x;
}
// Get the previous frame's Y mouse position
long InputManager::GetPreviousMouseY()
{
	return prevMousePos.y;
}

// Get the current scrollwheel delta
float InputManager::GetScrollWheelDelta()
{
	return wheelDelta;
}

// Get the X coordinate of the window (left)
long InputManager::GetWindowX()
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	return rect.left;
}

// Get the Y coordinate of the window (top)
long InputManager::GetWindowY()
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	return rect.top;
}

// Get the center X of the window
long InputManager::GetWindowCenterX()
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	return rect.left + ((rect.right - rect.left) / 2);
}

// Get the center Y of the window
long InputManager::GetWindowCenterY()
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	return rect.top + ((rect.bottom - rect.top) / 2);
}

long InputManager::GetWindowWidth()
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	return (rect.right - rect.left) / 2;
}

long InputManager::GetWindowHeight()
{
	RECT rect;
	GetWindowRect(hWnd, &rect);
	return (rect.right - rect.left) / 2;
}
