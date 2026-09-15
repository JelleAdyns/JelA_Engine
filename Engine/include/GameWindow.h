#ifndef WINDOW_H
#define WINDOW_H
#include <functional>

#include "Structs.h"

namespace jela
{
    class GameWindow
    {
    public:

        using MessageResult = std::optional<LRESULT>;
        using MessageHandler = std::function<MessageResult(HWND, UINT, WPARAM, LPARAM)>;

        GameWindow(int width, int height, HINSTANCE hInstance, const tstring& title, const tstring& dataPath, MessageHandler&& messageHandler);
        ~GameWindow();

        GameWindow(const GameWindow&) = delete;
        GameWindow(GameWindow&&) noexcept = delete;
        GameWindow& operator=(const GameWindow&) = delete;
        GameWindow& operator=(GameWindow&&) noexcept = delete;

        // Setters

        void ShowMouse(bool show) const;
        void SetWindowDimensions(int width, int height, bool refreshWindowPos = true);
        void SetWindowScale(float scale);

        // Getters

        Vector2f GetGameSize() const;
        Point2f GetViewportPos() const;
        Vector2f GetViewportSize() const;
        Point2f GetWindowPos() const;
        Vector2f GetWindowSize() const;
        float GetWindowScale() const;
        float GetWindowPosOffset() const;
        HWND GetWindow() const;
        bool IsWindowActive() const;
        bool IsFullscreen() const;

        void SetWindowPosition(bool setPos, bool setSize);
        void SetFullscreen();
        void CalculateWindowPos();
    private:
        static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        MessageResult HandleMessages(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

        HWND                            m_hWindow;
        DWORD                           m_OriginalStyle{ WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX };

        float                           m_WindowScale{ 1 };
        int                             m_GameWidth{};
        int                             m_GameHeight{};
        int                             m_WindowWidth{};
        int                             m_WindowHeight{};
        float                           m_ViewPortWidth{};
        float                           m_ViewPortHeight{};
        float                           m_ViewPortTranslationX{};
        float                           m_ViewPortTranslationY{};
        float                           m_MinScale{};
        int                             m_WindowPosX{};
        int                             m_WindowPosY{};
        static constexpr int            m_WindowPosOffset{5};

        bool                            m_IsFullscreen{};
        bool                            m_WindowIsActive{ true };

        MessageHandler                  m_MessageHandler{};
    };

} // jela

#endif //WINDOW_H
