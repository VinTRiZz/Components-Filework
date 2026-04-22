
if (${COMPONENTS_IS_ENABLED_QT})
    COMPONENTS_CONFIGURE_COMPONENT(Filework
        Qt5::Core
        stdc++fs
    )
else()
    COMPONENTS_CONFIGURE_COMPONENT(Filework
        stdc++fs
    )
endif()

COMPONENTS_LINK_COMPONENT(Filework Logger)
COMPONENTS_LINK_COMPONENT(Filework Common)
COMPONENTS_LINK_COMPONENT(Filework ExtraClasses)