#!/usr/bin/env python3
import glob
import argparse
import sys
import os
import time
import subprocess
import re
import tempfile
import platform
from shutil import which
from ninja import BIN_DIR
# local copy as the pip version doesn't have dyndeps in build() func
import ninja_syntax

os.environ['WINEDEBUG'] = '-all'
os.environ['TMPDIR'] = tempfile.gettempdir()

has_wine = bool(which('wine'))
has_wibo = bool(which('wibo'))
has_cpp = bool(which('cpp'))

# Native preprocesor doesn't work under WSL
if "microsoft-standard" in platform.uname().release:
    has_cpp = False

# TODO: make r3000.h and asm.h case sensitive symlinks on linux

def parse_arguments():
    parser = argparse.ArgumentParser(description='MGS Ninja build script generator')

    # Optional
    parser.add_argument('--psyq_path', type=str, default=os.environ.get("PSYQ_SDK") or "../../psyq_sdk",
                        help='Path to the root of the cloned PSYQ repo')

    args = parser.parse_args()

    args.psyq_path = os.path.relpath(args.psyq_path).replace("\\","/")
    args.obj_directory = 'obj'
    args.defines = []
    print("psyq_path = " + args.psyq_path)
    return args

def prefix(pfx, cmd):
    if pfx == "wibo" and has_wibo:
        return f"wibo {cmd}"
    if has_wine:
        return f"wine {cmd}"
    return cmd

def ninja_run():
    ninja = os.path.join(BIN_DIR, 'ninja')
    ninja_args = [] # TODO: pass through args to ninja?
    return subprocess.call([ninja, "--verbose"] + ninja_args)

args = parse_arguments()

f = open("build.ninja", "w")
ninja = ninja_syntax.Writer(f)

ninja.variable("psyq_path", args.psyq_path)
ninja.newline()

ninja.variable("target_name", "default")
ninja.newline()

ninja.variable("suffix", "")
ninja.newline()

ninja.variable("psyq_path_backslashed", args.psyq_path.replace('/', '\\'))
ninja.newline()

ninja.variable("psyq_asmpsx_44_exe", prefix("wibo", "$psyq_path/psyq_4.4/bin/asmpsx.exe"))
ninja.newline()

preprocessor_defines = ' '.join(f'-D{define}' for define in args.defines)

if has_cpp:
    ninja.variable("psyq_c_preprocessor_44_exe", f"cpp -nostdinc {preprocessor_defines}")
else:
    ninja.variable("psyq_c_preprocessor_44_exe", prefix("wine", f"$psyq_path/psyq_4.4/bin/CPPPSX.exe {preprocessor_defines}"))
ninja.newline()

ninja.variable("psyq_cc_44_exe", prefix("wine", "$psyq_path/psyq_4.4/bin/CC1PLPSX.EXE"))
ninja.newline()

ninja.variable("psyq_aspsx_44_exe", prefix("wibo", "$psyq_path/psyq_4.4/bin/aspsx.exe"))
ninja.newline()

ninja.variable("psyq_psylink_exe", prefix("wibo", "$psyq_path/psyq_4.4/bin/psylink.exe"))
ninja.newline()

ninja.variable("src_dir", "..")
ninja.newline()

# /l = produce linkable output file
# /q = run quietly
ninja.rule("psyq_asmpsx_assemble", "$psyq_asmpsx_44_exe /l /q $in,$out", "Assemble $in -> $out")
ninja.newline()

includes = "-I " + args.psyq_path + "/psyq_4.4/INCLUDE" + " -I $src_dir"

ninja.rule("psyq_c_preprocess_44", "$psyq_c_preprocessor_44_exe -undef $is_test -DPSX -D__GNUC__=2 -D__OPTIMIZE__ " + includes + " -lang-c -Dmips -D__mips__ -D__mips -Dpsx -D__psx__ -D__psx -D_PSYQ -D__EXTENSIONS__ -D_MIPSEL -D__CHAR_UNSIGNED__ -D_LANGUAGE_C -D__cplusplus -D_LANGUAGE_C_PLUS_PLUS $in $out", "Preprocess $in -> $out")
ninja.newline()

# generate header deps, adds edges to the build graph for the next build -M option will write header deps
ninja.rule("psyq_c_preprocess_44_headers", "$psyq_c_preprocessor_44_exe -M -undef $is_test -DPSX -D_LANGUAGE_C_PLUS_PLUS -D__cplusplus -D__GNUC__=2 -D__OPTIMIZE__ " + includes + " -lang-c -Dmips -D__mips__ -D__mips -Dpsx -D__psx__ -D__psx -D_PSYQ -D__EXTENSIONS__ -D_MIPSEL -D__CHAR_UNSIGNED__  $in $out", "Preprocess for includes $in -> $out")
ninja.newline()

ninja.rule("header_deps", f"{sys.executable} hash_include_msvc_formatter.py $in $out", "Include deps fix $in -> $out", deps="msvc")
ninja.newline()

ninja.rule("asm_include_preprocess_44", f"{sys.executable} include_asm_preprocess.py $in $out", "Include asm preprocess $in -> $out")
ninja.newline()

ninja.rule("asm_include_postprocess", f"{sys.executable} include_asm_fixup.py $in $out", "Include asm postprocess $in -> $out")
ninja.newline()

ninja.variable("gSize", "8")
ninja.newline()

ninja.rule("psyq_cc_44", "$psyq_cc_44_exe -quiet -O2 -w -G $gSize -g -Wall $in -o $out""", "Compile $in -> $out")
ninja.newline()

ninja.rule("psyq_aspsx_assemble_44", "$psyq_aspsx_44_exe -q $in -o $out", "Assemble $in -> $out")
ninja.newline()

ninja.rule("linker_command_file_preprocess", f"{sys.executable} linker_command_file_preprocess.py $in $psyq_sdk $out {' '.join(args.defines)} $overlay $overlay_suffix", "Preprocess $in -> $out")
ninja.newline()

psqy_lib = f'{args.psyq_path}/psyq_4.4/LIB'

ninja.rule("psylink", f"$psyq_psylink_exe /l {psqy_lib} /c /n 4000 /q /gp .sdata /m @\"../{args.obj_directory}/$target_name/linker_command_file$suffix.txt\",../{args.obj_directory}/$target_name/sound$suffix.cpe,../{args.obj_directory}/$target_name/asm$suffix.sym,../{args.obj_directory}/$target_name/asm$suffix.map", "Link $out")

# TODO: update the tool so we can set the output name optionally
# cmd /c doesn't like forward slashed relative paths
ninja.rule("cpe2exe", prefix("wine", "cmd /c \"$psyq_path_backslashed\\psyq_4.3\\bin\\cpe2exe.exe -CJ $in > NUL\""), "cpe2exe $in -> $out")
ninja.newline()

def create_psyq_ini(sdkDir, psyqDir):
    data = ""
    with open(sdkDir + "/" + psyqDir + "/bin/psyq.ini.template", 'r') as file:
        data = file.read()
    data = data.replace("$PSYQ_PATH", sdkDir + "/" + psyqDir)
    data = data.replace("/", "\\")
    ini_file = open(sdkDir + "/" + psyqDir + "/bin/psyq.ini", "w")
    ini_file.write(data)

def init_psyq_ini_files(sdkDir):
    create_psyq_ini(sdkDir, "psyq_4.3")
    create_psyq_ini(sdkDir, "psyq_4.4")

def get_files_recursive(path, ext):
    collectedFiles = []
    # r=root, d=directories, f = files
    for r, d, f in os.walk(path):
        for file in f:
            if file.endswith(ext):
                collectedFiles.append(os.path.join(r, file))
    return collectedFiles

def gen_build_target(targetName, is_test):
    ninja.comment("Build target " + targetName)

    asmFiles = get_files_recursive("../asm", ".s")
    print("Got " + str(len(asmFiles)) + " asm files")

    cFiles = ['../vs_vt.c', '../vs_vtc.c', '../main.cpp', '../lib_snd.cpp', '../lib_spu.cpp', '../test.cpp']
    print("Got " + str(len(cFiles)) + " source files")

    linkerDeps = []

    # TODO: Use the correct toolchain and -G flag for each c file
    # TODO: .h file deps of .c files

    # build .s files
    for asmFile in asmFiles:
        asmFile = asmFile.replace("\\", "/")
        asmOFile = asmFile.replace("/asm/", f"/{args.obj_directory}/{targetName}/")
        asmOFile = asmOFile.replace(".s", ".obj")
        #print("Build step " + asmFile + " -> " + asmOFile)
        ninja.build(asmOFile, "psyq_asmpsx_assemble", asmFile)
        linkerDeps.append(asmOFile)

    # build .c files
    for cFile in cFiles:
        cFile = cFile.replace("\\", "/")
        cOFile = cFile.replace("../", f"../{args.obj_directory}/{targetName}/")

        if 'cpp' in cFile:
            cPreProcHeadersFile = cOFile.replace(".cpp", ".cpp.preproc.headers")
            cPreProcHeadersFixedFile = cOFile.replace(".cpp", ".cpp.preproc.headers_fixed")
            cConvertedFile = cOFile.replace(".cpp", ".cpp.eucjp")
            cPreProcFile = cOFile.replace(".cpp", ".cpp.preproc")
            cDynDepFile = cOFile.replace(".cpp", ".cpp.dyndep")
            cAsmPreProcFile = cOFile.replace(".cpp", ".cpp.asm.preproc")
            cAsmPreProcFileDeps = cOFile.replace(".cpp", ".cpp.asm.preproc.deps")
            cAsmFile = cOFile.replace(".cpp", ".asm")
            cTempOFile = cOFile.replace(".cpp", ".cpp.fixme.obj")
            cOFile = cOFile.replace(".cpp", ".obj")
        else:
            cPreProcHeadersFile = cOFile.replace(".c", ".c.preproc.headers")
            cPreProcHeadersFixedFile = cOFile.replace(".c", ".c.preproc.headers_fixed")
            cConvertedFile = cOFile.replace(".c", ".c.eucjp")
            cPreProcFile = cOFile.replace(".c", ".c.preproc")
            cDynDepFile = cOFile.replace(".c", ".c.dyndep")
            cAsmPreProcFile = cOFile.replace(".c", ".c.asm.preproc")
            cAsmPreProcFileDeps = cOFile.replace(".c", ".c.asm.preproc.deps")
            cAsmFile = cOFile.replace(".c", ".asm")
            cTempOFile = cOFile.replace(".c", ".c.fixme.obj")
            cOFile = cOFile.replace(".c", ".obj")
        #print("Build step " + asmFile + " -> " + asmOFile)

        ninja.build(cPreProcHeadersFile, "psyq_c_preprocess_44_headers", cFile)
        ninja.build(cPreProcHeadersFixedFile, "header_deps", cPreProcHeadersFile)

        compiler = "psyq_cc_44"
        aspsx = "psyq_aspsx_assemble_44"
        if(is_test):
            is_test_def = "-DUNIT_TESTS"
        else:
            is_test_def = ""
        ninja.build(cPreProcFile, "psyq_c_preprocess_44", cFile, implicit=[cPreProcHeadersFixedFile], variables={'is_test':is_test_def})
        ninja.build([cAsmPreProcFile, cAsmPreProcFileDeps, cDynDepFile], "asm_include_preprocess_44", cPreProcFile)
        ninja.build(cAsmFile, compiler, cAsmPreProcFile)
        ninja.build(cTempOFile, aspsx, cAsmFile)
        ninja.build(cOFile, "asm_include_postprocess", cTempOFile, implicit=[cAsmPreProcFileDeps, cDynDepFile], dyndep=cDynDepFile)

        linkerDeps.append(cOFile)

    # Build main exe

    # preprocess linker_command_file.txt
    linkerCommandFile = f"../{args.obj_directory}/{targetName}/linker_command_file.txt"
    if(is_test):
        ninja.build(linkerCommandFile, "linker_command_file_preprocess", f"linker_command_file_test.txt", variables={'psyq_sdk': args.psyq_path})
    else:
        ninja.build(linkerCommandFile, "linker_command_file_preprocess", f"linker_command_file.txt", variables={'psyq_sdk': args.psyq_path})
    ninja.newline()

    # run the linker to generate the cpe
    cpeFile = f"../{args.obj_directory}/{targetName}/sound.cpe"
    ninja.build(cpeFile, "psylink", implicit=linkerDeps + [linkerCommandFile], variables={'target_name':targetName})
    ninja.newline()

    # cpe to exe
    exeFile = f"../{args.obj_directory}/{targetName}/sound.exe"
    ninja.build(exeFile, "cpe2exe", cpeFile)
    ninja.newline()

gen_build_target("psx_seq_player", False)
gen_build_target("psx_seq_player_test", True)

f.close()

time_before = time.time()
exit_code = ninja_run()
took = time.time() - time_before
print(f'build took {took:.2f} seconds')

sys.exit(exit_code)
