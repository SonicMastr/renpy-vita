#include <stdlib.h>
#include <string.h>
#include <Python.h>
#include <SDL2/SDL.h>
#include <psp2/sysmodule.h>
#include <psp2/power.h>
#include <psp2/appmgr.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/vshbridge.h> 
#include <gpu_es4/psp2_pvr_hint.h>

#define MAX_PATH 256

int _newlib_heap_size_user = 180 * 1024 * 1024;
unsigned int sceLibcHeapSize = 10 * 1024 * 1024;

PyMODINIT_FUNC initpygame_sdl2_color();
PyMODINIT_FUNC initpygame_sdl2_controller();
PyMODINIT_FUNC initpygame_sdl2_display();
PyMODINIT_FUNC initpygame_sdl2_draw();
PyMODINIT_FUNC initpygame_sdl2_error();
PyMODINIT_FUNC initpygame_sdl2_event();
#if 0
PyMODINIT_FUNC initpygame_sdl2_font();
#endif
PyMODINIT_FUNC initpygame_sdl2_gfxdraw();
PyMODINIT_FUNC initpygame_sdl2_image();
PyMODINIT_FUNC initpygame_sdl2_joystick();
PyMODINIT_FUNC initpygame_sdl2_key();
PyMODINIT_FUNC initpygame_sdl2_locals();
#if 0
PyMODINIT_FUNC initpygame_sdl2_mixer();
PyMODINIT_FUNC initpygame_sdl2_mixer_music();
#endif
PyMODINIT_FUNC initpygame_sdl2_mouse();
PyMODINIT_FUNC initpygame_sdl2_power();
PyMODINIT_FUNC initpygame_sdl2_pygame_time();
PyMODINIT_FUNC initpygame_sdl2_rect();
PyMODINIT_FUNC initpygame_sdl2_render();
PyMODINIT_FUNC initpygame_sdl2_rwobject();
PyMODINIT_FUNC initpygame_sdl2_scrap();
PyMODINIT_FUNC initpygame_sdl2_surface();
PyMODINIT_FUNC initpygame_sdl2_transform();
PyMODINIT_FUNC init_renpy();
PyMODINIT_FUNC init_renpybidi();
PyMODINIT_FUNC initrenpy_audio_renpysound();
PyMODINIT_FUNC initrenpy_display_accelerator();
PyMODINIT_FUNC initrenpy_display_render();
PyMODINIT_FUNC initrenpy_display_matrix();
PyMODINIT_FUNC initrenpy_gl_gldraw();
PyMODINIT_FUNC initrenpy_gl_glenviron_shader();
PyMODINIT_FUNC initrenpy_gl_glrtt_copy();
PyMODINIT_FUNC initrenpy_gl_glrtt_fbo();
PyMODINIT_FUNC initrenpy_gl_gltexture();
PyMODINIT_FUNC initrenpy_gl2_gl2draw();
PyMODINIT_FUNC initrenpy_gl2_gl2mesh();
PyMODINIT_FUNC initrenpy_gl2_gl2mesh2();
PyMODINIT_FUNC initrenpy_gl2_gl2mesh3();
PyMODINIT_FUNC initrenpy_gl2_gl2model();
PyMODINIT_FUNC initrenpy_gl2_gl2polygon();
PyMODINIT_FUNC initrenpy_gl2_gl2shader();
PyMODINIT_FUNC initrenpy_gl2_gl2texture();
PyMODINIT_FUNC initrenpy_parsersupport();
PyMODINIT_FUNC initrenpy_pydict();
PyMODINIT_FUNC initrenpy_style();
PyMODINIT_FUNC initrenpy_styledata_style_activate_functions();
PyMODINIT_FUNC initrenpy_styledata_style_functions();
PyMODINIT_FUNC initrenpy_styledata_style_hover_functions();
PyMODINIT_FUNC initrenpy_styledata_style_idle_functions();
PyMODINIT_FUNC initrenpy_styledata_style_insensitive_functions();
PyMODINIT_FUNC initrenpy_styledata_style_selected_activate_functions();
PyMODINIT_FUNC initrenpy_styledata_style_selected_functions();
PyMODINIT_FUNC initrenpy_styledata_style_selected_hover_functions();
PyMODINIT_FUNC initrenpy_styledata_style_selected_idle_functions();
PyMODINIT_FUNC initrenpy_styledata_style_selected_insensitive_functions();
PyMODINIT_FUNC initrenpy_styledata_styleclass();
PyMODINIT_FUNC initrenpy_styledata_stylesets();
PyMODINIT_FUNC initrenpy_styledata_styleutil();
PyMODINIT_FUNC initrenpy_text_ftfont();
PyMODINIT_FUNC initrenpy_text_textsupport();
PyMODINIT_FUNC initrenpy_text_texwrap();
PyMODINIT_FUNC initrenpy_uguu_gl();
PyMODINIT_FUNC initrenpy_uguu_uguu();

char app_dir_path[0x100];
char res_1080_path[0x100];
char res_720_path[0x100];
char app_gles_module_path[0x100];
char app_program_path[0x100];
char relative_dir_path[0x400];
char sysconfigdata_file_path[0x400];
char python_home_buffer[0x400];
char python_snprintf_buffer[0x400];
char python_script_buffer[0x400];
char python_error_buffer[0x400];
char title_id[0xA];

SceUID pid = -1;

void show_error_and_exit(const char* message)
{
    Py_Finalize();
    printf("%s", message);
    Py_Exit(1);
}

int main(int argc, char* argv[])
{
    PVRSRV_PSP2_APPHINT hint;
    SceUID fd = -1;
    SceBool override = SCE_FALSE;
    char target_path[MAX_PATH];

    Py_NoSiteFlag = 1;
    Py_IgnoreEnvironmentFlag = 1;
    Py_NoUserSiteDirectory = 1;
    Py_OptimizeFlag = 2;

    sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_OLED_DIMMING | SCE_KERNEL_POWER_TICK_DISABLE_OLED_OFF);
    /* Ren'Py is a bit CPU heavy. Increase CPU clocks */
    scePowerSetArmClockFrequency(444);
    /* Ren'Py requires full RW access */
    sceAppMgrUmount("app0:");

    /* Because locations can be all over the place, get TITLEID to find the application directory */
    pid = sceKernelGetProcessId();
    sceAppMgrAppParamGetString(pid, 12, title_id, sizeof(title_id));

    /* Generate full path to application directory and set relative paths */
    snprintf(app_dir_path, sizeof(app_dir_path), "ux0:app/%s", title_id);
    snprintf(res_1080_path, sizeof(res_1080_path), "%s/1080", app_dir_path);
    snprintf(res_720_path, sizeof(res_720_path), "%s/720", app_dir_path);
    snprintf(app_gles_module_path, sizeof(app_gles_module_path), "%s/module", app_dir_path);
    snprintf(app_program_path, sizeof(app_program_path), "%s/eboot.bin", app_dir_path);

    /* Disable Back Touchpad to prevent "misclicks" */
    SDL_setenv("VITA_DISABLE_TOUCH_BACK", "1", 1);

    /* Check Custom Resolutions */
    /* Alllow 1080i and 720p support on both PSTV and Vita if wanted (Needs Sharpscale) */
    if (fd = sceIoOpen(res_1080_path, SCE_O_RDONLY, 0777) > 0) {
        sceIoClose(fd);
        override = SCE_TRUE;
        /* This actually needs it for both systems,
         * so just set it here for the vita */
        scePowerSetGpuClockFrequency(222); 
        SDL_setenv("VITA_RESOLUTION", "1080", 1);
    }
    else if (fd = sceIoOpen(res_720_path, SCE_O_RDONLY, 0777) > 0) {
        sceIoClose(fd);
        override = SCE_TRUE;
        /* Vita doesn't really need it, but could help */
        scePowerSetGpuClockFrequency(166);
        SDL_setenv("VITA_RESOLUTION", "720", 1);
    }
    /* At least Set 720p for PSTV and force 222MHz GPU since it's running on AC anyway */
    if (vshSblAimgrIsDolce()) {
        scePowerSetGpuClockFrequency(222);
        if (!override)
            SDL_setenv("VITA_RESOLUTION", "720", 1);
    }

    /* We need to use some custom hints */
    SDL_setenv("VITA_PVR_SKIP_INIT", "yeet", 1);

    /* Load Modules */
    sceKernelLoadStartModule("vs0:sys/external/libfios2.suprx", 0, NULL, 0, NULL, NULL);
    sceKernelLoadStartModule("vs0:sys/external/libc.suprx", 0, NULL, 0, NULL, NULL);
    snprintf(target_path, MAX_PATH, "%s/%s", app_gles_module_path, "libgpu_es4_ext.suprx");
    sceKernelLoadStartModule(target_path, 0, NULL, 0, NULL, NULL);
    snprintf(target_path, MAX_PATH, "%s/%s", app_gles_module_path, "libIMGEGL.suprx");
    sceKernelLoadStartModule(target_path, 0, NULL, 0, NULL, NULL);

    /* Set PVR Hints */
    PVRSRVInitializeAppHint(&hint);
    snprintf(hint.szGLES1, MAX_PATH, "%s/%s", app_gles_module_path, "libGLESv1_CM.suprx");
    snprintf(hint.szGLES2, MAX_PATH, "%s/%s", app_gles_module_path, "libGLESv2.suprx");
    snprintf(hint.szWindowSystem, MAX_PATH, "%s/%s", app_gles_module_path, "libpvrPSP2_WSEGL.suprx");
    hint.ui32SwTexOpCleanupDelay = 2000; // Set to two milliseconds to prevent a pool of unfreed memory
    PVRSRVCreateVirtualAppHint(&hint);

    /* Now let's start the Ren'Py Process */
    Py_SetProgramName(app_program_path);

    static struct _inittab builtins[] = {

        {"pygame_sdl2.color", initpygame_sdl2_color},
        {"pygame_sdl2.controller", initpygame_sdl2_controller},
        {"pygame_sdl2.display", initpygame_sdl2_display},
        {"pygame_sdl2.draw", initpygame_sdl2_draw},
        {"pygame_sdl2.error", initpygame_sdl2_error},
        {"pygame_sdl2.event", initpygame_sdl2_event},
#if 0
        {"pygame_sdl2.font", initpygame_sdl2_font},
#endif
        {"pygame_sdl2.gfxdraw", initpygame_sdl2_gfxdraw},
        {"pygame_sdl2.image", initpygame_sdl2_image},
        {"pygame_sdl2.joystick", initpygame_sdl2_joystick},
        {"pygame_sdl2.key", initpygame_sdl2_key},
        {"pygame_sdl2.locals", initpygame_sdl2_locals},
#if 0
        {"pygame_sdl2.mixer", initpygame_sdl2_mixer},
        {"pygame_sdl2.mixer_music", initpygame_sdl2_mixer_music},
#endif
        {"pygame_sdl2.mouse", initpygame_sdl2_mouse},
        {"pygame_sdl2.power", initpygame_sdl2_power},
        {"pygame_sdl2.pygame_time", initpygame_sdl2_pygame_time},
        {"pygame_sdl2.rect", initpygame_sdl2_rect},
        {"pygame_sdl2.render", initpygame_sdl2_render},
        {"pygame_sdl2.rwobject", initpygame_sdl2_rwobject},
        {"pygame_sdl2.scrap", initpygame_sdl2_scrap},
        {"pygame_sdl2.surface", initpygame_sdl2_surface},
        {"pygame_sdl2.transform", initpygame_sdl2_transform},
        {"_renpy", init_renpy},
        {"_renpybidi", init_renpybidi},
        {"renpy.audio.renpysound", initrenpy_audio_renpysound},
        {"renpy.display.accelerator", initrenpy_display_accelerator},
        {"renpy.display.matrix", initrenpy_display_matrix},
        {"renpy.display.render", initrenpy_display_render},
        {"renpy.gl.gldraw", initrenpy_gl_gldraw},
        {"renpy.gl.glenviron_shader", initrenpy_gl_glenviron_shader},
        {"renpy.gl.glrtt_copy", initrenpy_gl_glrtt_copy},
        {"renpy.gl.glrtt_fbo", initrenpy_gl_glrtt_fbo},
        {"renpy.gl.gltexture", initrenpy_gl_gltexture},
        {"renpy.gl2.gl2draw", initrenpy_gl2_gl2draw},
        {"renpy.gl2.gl2mesh", initrenpy_gl2_gl2mesh},
        {"renpy.gl2.gl2mesh2", initrenpy_gl2_gl2mesh2},
        {"renpy.gl2.gl2mesh3", initrenpy_gl2_gl2mesh3},
        {"renpy.gl2.gl2model", initrenpy_gl2_gl2model},
        {"renpy.gl2.gl2polygon", initrenpy_gl2_gl2polygon},
        {"renpy.gl2.gl2shader", initrenpy_gl2_gl2shader},
        {"renpy.gl2.gl2texture", initrenpy_gl2_gl2texture},
        {"renpy.parsersupport", initrenpy_parsersupport},
        {"renpy.pydict", initrenpy_pydict},
        {"renpy.style", initrenpy_style},
        {"renpy.styledata.style_activate_functions", initrenpy_styledata_style_activate_functions},
        {"renpy.styledata.style_functions", initrenpy_styledata_style_functions},
        {"renpy.styledata.style_hover_functions", initrenpy_styledata_style_hover_functions},
        {"renpy.styledata.style_idle_functions", initrenpy_styledata_style_idle_functions},
        {"renpy.styledata.style_insensitive_functions", initrenpy_styledata_style_insensitive_functions},
        {"renpy.styledata.style_selected_activate_functions", initrenpy_styledata_style_selected_activate_functions},
        {"renpy.styledata.style_selected_functions", initrenpy_styledata_style_selected_functions},
        {"renpy.styledata.style_selected_hover_functions", initrenpy_styledata_style_selected_hover_functions},
        {"renpy.styledata.style_selected_idle_functions", initrenpy_styledata_style_selected_idle_functions},
        {"renpy.styledata.style_selected_insensitive_functions", initrenpy_styledata_style_selected_insensitive_functions},
        {"renpy.styledata.styleclass", initrenpy_styledata_styleclass},
        {"renpy.styledata.stylesets", initrenpy_styledata_stylesets},
        {"renpy.text.ftfont", initrenpy_text_ftfont},
        {"renpy.text.textsupport", initrenpy_text_textsupport},
        {"renpy.text.texwrap", initrenpy_text_texwrap},
        {"renpy.uguu.gl", initrenpy_uguu_gl},
        {"renpy.uguu.uguu", initrenpy_uguu_uguu},

        {NULL, NULL}
    };

    getcwd(relative_dir_path, sizeof(relative_dir_path));

    char* dir_paths[] = {
        app_dir_path,
        relative_dir_path,
        NULL,
    };

    int found_sysconfigdata = 0;
    int found_renpy = 0;

    for (int i = 0; i < sizeof(dir_paths); i += 1)
    {
        if (dir_paths[i] == NULL)
        {
            break;
        }
        snprintf(sysconfigdata_file_path, sizeof(sysconfigdata_file_path), "%s/lib/python27.zip", dir_paths[i]);
        FILE* sysconfigdata_file = fopen((const char*)sysconfigdata_file_path, "rb");
        if (sysconfigdata_file != NULL)
        {
            found_sysconfigdata = 1;
            fclose(sysconfigdata_file);
        }

        snprintf(python_script_buffer, sizeof(python_script_buffer), "%s/renpy.py", dir_paths[i]);
        FILE* renpy_file = fopen((const char*)python_script_buffer, "rb");
        if (renpy_file != NULL)
        {
            found_renpy = 1;
            fclose(renpy_file);
        }

        if (found_sysconfigdata == 1 && found_renpy == 1)
        {
            snprintf(python_home_buffer, sizeof(python_home_buffer), "%s/lib/python27.zip", dir_paths[i]);
            snprintf(python_snprintf_buffer, sizeof(python_snprintf_buffer), "import sys\nsys.path = ['%s/lib/python27.zip']", dir_paths[i]);
            Py_SetPythonHome(python_home_buffer);
            break;
        }
    }

    if (found_sysconfigdata == 0)
    {
        show_error_and_exit("Could not find python27.zip.\n");
    }

    if (found_renpy == 0)
    {
        show_error_and_exit("Could not find renpy.py.\n");
    }

    Py_InitializeEx(0);

    PyImport_ExtendInittab(builtins);

    PyObject *pmodule;

    char* pyargs[] = {
        python_script_buffer,
        app_dir_path,
        NULL,
    };

    PySys_SetArgvEx(2, pyargs, 1);

    int python_result;

    python_result = PyRun_SimpleString(python_snprintf_buffer);

    if (python_result == -1)
    {
        show_error_and_exit("Could not set the Python path.\n\nThis is an internal error and should not occur during normal usage.");
    }

#define x(lib) \
    { \
        if (PyRun_SimpleString("import " lib) == -1) \
        { \
            show_error_and_exit("Could not import python library " lib ".\n"); \
        } \
    }

    x("os");
    x("pygame_sdl2");
    x("encodings");

#undef x

    FILE* renpy_file = fopen((const char*)python_script_buffer, "rb");
    if (renpy_file == NULL)
    {
        show_error_and_exit("Could not open renpy.py after Python initialization.\n\nThis is an internal error and should not occur during normal usage.");
    }
    else
    {
        /* This is where the fun begins */
        python_result = PyRun_SimpleFileEx(renpy_file, (const char*)python_script_buffer, 1);
    }

    if (python_result == -1)
    {
        show_error_and_exit("An uncaught Python exception occurred during renpy.py execution.\n\nPlease look in the \"Ren'Py Logs\" folder on the SD card root for more information about this exception.");
    }

    Py_Exit(0);
    return 0;
}