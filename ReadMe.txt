SD7 A3 Readme:

How to use:
  Because I didn't have a project existing for this assignment, I used my thesis project as a base and wrote some text functions on top.

  Go into TheGame.cpp and look at the top of the file. You should see the following code:


                                void Memes(NamedProperties& properties)
                                {
                                    DebuggerPrintf("ayyylmao");
                                    properties.Set("maymay", 82);
                                }

                                class EventTestObject
                                {
                                public:
                                    ~EventTestObject()
                                    {
                                        EventSystem::UnregisterFromAllEvents(this);
                                    }
                                    void MAAAYMES(NamedProperties& properties)
                                    {
                                        DebuggerPrintf("ayyylmao");
                                        int meaymayean;
                                        properties.Get("maymay", meaymayean);
                                        std::string hurhhhhh;
                                        properties.Get("ayyy", hurhhhhh);
                                    }
                                };

                                //-----------------------------------------------------------------------------------
                                TheGame::TheGame()
                                    : m_currentGameMode(nullptr)
                                {
                                    ResourceDatabase::instance = new ResourceDatabase();
                                    RegisterSprites();
                                    SetGameState(GameState::MAIN_MENU);
                                    InitializeMainMenuState();
                                    NamedProperties props("maymay", "lmao");
                                    EventSystem::RegisterEventCallback("memes", &Memes, "Memes are great");
                                    EventTestObject* test = new EventTestObject();
                                    EventSystem::RegisterObjectForEvent("memes", test, &EventTestObject::MAAAYMES);
                                    EventSystem::FireEvent("memes", props);
                                    delete test;
                                    EnumerateFiles("Logs", "*", true, "memes");
                                }



This is the test code for the functionality I added in A3. The above tests getting and setting of named properties, passing them into functions, overwriting them, etc.
I previously checked the error codes, but I lost some of them while hacking back and forth. Feel free to use this part of the code to test all of my functionality and
verify that everything works, and don't worry about breaking anything.

I'm going to try to push a breakpoint for you to help aid testing, but I'm not sure if it'll get committed. We'll find out :)


Issues:
  Because we're running in release, you WILL have about 52 bytes leaked from things that I can't control. The callstacks should match the following:

                                ------------------------------------------------------------------------------
                                RUN-TIME RECOVERABLE WARNING on line 215 of MemoryTracking.cpp, in MemoryAnalyticsShutdown()
                                Core\Memory\MemoryTracking.cpp(215): Leaked a total of 52 bytes, from 2 individual leaks. 1 of these were from before startup.
                                [Press enter to continue]
                                ------------------------------------------------------------------------------

                                ---===Allocation #0===---
                                >>>Size: 44 bytes
                                >>>Callstack:
                                //-----------------------------------------------------------------------------------
                                c:\program files (x86)\microsoft visual studio 14.0\vc\include\xmemory0(90): std::_Allocate
                                N/A(0): initterm
                                f:\dd\vctools\crt\vcstartup\src\startup\exe_common.inl(230): __scrt_common_main_seh
                                N/A(0): BaseThreadInitThunk
                                N/A(0): RtlSubscribeWnfStateChangeNotification
                                N/A(0): RtlSubscribeWnfStateChangeNotification
                                //-----------------------------------------------------------------------------------

                                ---===Allocation #1===---
                                >>>Size: 8 bytes
                                >>>Callstack:
                                //-----------------------------------------------------------------------------------
                                d:\guildhall\p4\c25\students\acloudy\projects\engine\code\engine\core\memory\memorytracking.cpp(18): operator new
                                f:\dd\vctools\crt\crtw32\stdcpp\locale0.cpp(73): std::_Facet_Register
                                c:\program files (x86)\microsoft visual studio 14.0\vc\include\xlocale(592): std::use_facet<std::codecvt<char,char,_Mbstatet> >
                                c:\program files (x86)\microsoft visual studio 14.0\vc\include\fstream(271): std::basic_filebuf<char,std::char_traits<char> >::open
                                c:\program files (x86)\microsoft visual studio 14.0\vc\include\fstream(756): std::basic_ifstream<char,std::char_traits<char> >::basic_ifstream<char,std::char_traits<char> >
                                d:\guildhall\p4\c25\students\acloudy\projects\engine\code\engine\input\inputoutpututils.cpp(259): FileExists
                                d:\guildhall\p4\c25\students\acloudy\projects\engine\code\engine\fonts\bitmapfont.cpp(66): BitmapFont::CreateOrGetFont
                                d:\guildhall\p4\c25\students\acloudy\projects\engine\code\engine\renderer\renderer.cpp(61): Renderer::Renderer
                                d:\guildhall\p4\c25\students\acloudy\projects\thesis\allstar\code\game\main_win32.cpp(228): Initialize
                                d:\guildhall\p4\c25\students\acloudy\projects\thesis\allstar\code\game\main_win32.cpp(271): WinMain
                                f:\dd\vctools\crt\vcstartup\src\startup\exe_common.inl(264): __scrt_common_main_seh
                                N/A(0): BaseThreadInitThunk
                                N/A(0): RtlSubscribeWnfStateChangeNotification
                                N/A(0): RtlSubscribeWnfStateChangeNotification
                                //-----------------------------------------------------------------------------------


    These exist as issues from within the codebase, and some are known issues in visual studio. If you'd like to rebuild in debug, you shouldn't find any leaks
