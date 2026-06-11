#include <stdio.h>
#include <stdlib.h>

#include <LUA/LUAheader.h>

/*int main(int argc, const char** pArgs){
   printf("hhesef");

   lua_State* L = lua_open();
   return  EXIT_SUCCESS;
}*/


int main() {
    // stdin is required to build libssh2. closing it causes the following error:
    //    ../libtool: line 3291: 0: Bad file descriptor
    //    libtool:   error: 'channel.lo' is not a valid libtool object

    // ::fclose(stdin);

    lua_State *L = luaL_newstate();
    if (L == NULL) {
        printf("Failed to create Lua state.\n");
        return 1;
    }
    luaL_openlibs(L);

    // Raw string literal, requires C++11
    const char *luaCode = R"(
function BULrun(Dir, Command)
   local command = "cd "..Dir.." && "..Command
   local P = io.popen(command)
   while true do
      local output = P:read(1024)
      if not output then break end
      print(output)
   end
   print "done"
end
BULrun("/Users/jqi/build2/work/libcurl", "sh build.sh")
)";


    // Push the Lua string onto the stack
    lua_pushstring(L, luaCode);

    // Load the Lua chunk from the string and create a Lua function
    if (luaL_loadstring(L, luaCode) != 0) {
        printf("Error loading Lua code: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return 1;
    }

    // Call lua_pcall to execute the loaded Lua chunk (no arguments, no results)
    if (lua_pcall(L, 0, 0, 0) != 0) {
        printf("Error executing Lua code: %s\n", lua_tostring(L, -1));
        lua_close(L);
        return 1;
    }

    lua_close(L);
    return 0;
}
