
if (${COMPONENTS_IS_ENABLED_QT})
    COMPONENTS_CONFIGURE_COMPONENT(Filework
        Qt5::Core
        Qt5::Widgets
        stdc++fs
    )
else()
    COMPONENTS_CONFIGURE_COMPONENT(Filework
        stdc++fs
    )
endif()

COMPONENTS_LINK_COMPONENT(Filework Logger)
COMPONENTS_LINK_COMPONENT(Filework ExtraClasses)

COMPONENTS_ADD_COMPONENT_TEST(Filework)