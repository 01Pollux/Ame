target("HelloWorld")
    set_default(true)
    ame_utils:add_binary("Examples", "Source/Examples/HelloWorld")
target_end()

target("SimpleWindow")
    ame_utils:add_binary("Examples", "Source/Examples/SimpleWindow")
target_end()
