
if (${COMPONENTS_IS_ENABLED_QT})
    COMPONENTS_CONFIGURE_COMPONENT(Filework "1.0.0"
        Qt5::Core
        Qt5::Widgets
        stdc++fs
    )
else()
    COMPONENTS_CONFIGURE_COMPONENT(Filework "1.0.0"
        stdc++fs
    )
endif()

COMPONENTS_LINK_COMPONENT(Components-Filework Logger)
COMPONENTS_LINK_COMPONENT(Components-Filework ExtraClasses)

COMPONENTS_ADD_COMPONENT_TEST(Filework)