#!/usr/bin/env python3
"""Assemble and verify the actual Supermodel distribution (no runtime manifest)."""
import argparse
import ctypes
import ctypes.util
import hashlib
from pathlib import Path
import re
import shutil
import subprocess
import sys
import xml.etree.ElementTree as ET

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('core', type=Path)
parser.add_argument('output', type=Path)
parser.add_argument('--check', action='store_true')
args = parser.parse_args()
repo = Path(__file__).resolve().parents[2]
root = args.output.resolve()
name = args.core.name
required = {name, 'supermodel_libretro.info', 'LICENSE.txt', 'THIRD_PARTY_NOTICES.txt',
            'SOURCE_PROVENANCE.txt', 'SHA256SUMS', 'system/supermodel/Games.xml',
            'system/supermodel/Music.xml'}
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
if not args.check:
    assert name in {'supermodel_libretro.so', 'supermodel_libretro.dylib',
                    'supermodel_libretro.dll'}, name
    root.mkdir(parents=True, exist_ok=True)
    assert not any(root.iterdir()), 'use an empty package directory'
    for source, dest in [(args.core, name), (repo/'supermodel_libretro.info', 'supermodel_libretro.info'),
                         (repo/'Docs/LICENSE.txt', 'LICENSE.txt'),
                         (repo/'Docs/THIRD_PARTY_NOTICES.txt', 'THIRD_PARTY_NOTICES.txt'),
                         (repo/'Config/Games.xml', 'system/supermodel/Games.xml'),
                         (repo/'Config/Music.xml', 'system/supermodel/Music.xml')]:
        target = root/dest
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)
    git = lambda *a: subprocess.check_output(['git', '-C', str(repo), *a], text=True).strip()
    tracked = git('ls-files').splitlines()
    digest = hashlib.sha256()
    for path in sorted(tracked):
        source = repo/path
        if source.is_file():
            digest.update(path.encode()+b'\0'+bytes.fromhex(sha(source)))
    (root/'SOURCE_PROVENANCE.txt').write_text(
        'Repository: https://github.com/Zer0one/Libretro-Supermodel\n'
        f'Commit: {git("rev-parse", "HEAD")}\n'
        f'Tracked Working Tree SHA-256: {digest.hexdigest()}\n'
        f'Tracked Local Changes: {"Present" if git("diff", "--name-only", "HEAD") else "Absent"}\n'
        'Upstream Coverage: Selective; See Docs/UPSTREAM_INTEGRATION.md In Source\n', encoding='utf-8')
    (root/'SHA256SUMS').write_text(''.join(
        f'{sha(root/path)}  {path}\n' for path in sorted(required-{'SHA256SUMS'})), encoding='utf-8')
files = {p.relative_to(root).as_posix() for p in root.rglob('*') if p.is_file()}
assert files == required, f'package contents differ: {files ^ required}'
info = (root/'supermodel_libretro.info').read_text()
assert 'corename = "Supermodel"' in info and 'license = "GPLv3+"' in info
assert 'required_hw_api = "OpenGL >= 4.1|OpenGL ES >= 3.0"' in info
for filename in ['Games.xml', 'Music.xml']:
    ET.parse(root/'system/supermodel'/filename)
assert (root/name).stat().st_size > 0
assert 'GNU GENERAL PUBLIC LICENSE' in (root/'LICENSE.txt').read_text()
assert (root/'THIRD_PARTY_NOTICES.txt').stat().st_size > 0
assert re.search(r'^Commit: [0-9a-f]{40}$', (root/'SOURCE_PROVENANCE.txt').read_text(), re.M)
listed = {}
for line in (root/'SHA256SUMS').read_text().splitlines():
    digest, path = line.split('  ', 1)
    assert path not in listed and re.fullmatch('[0-9a-f]{64}', digest)
    listed[path] = digest
assert set(listed) == required-{'SHA256SUMS'}
for path, digest in listed.items():
    assert sha(root/path) == digest, f'checksum mismatch: {path}'
# Runtime identity is read without initializing emulation or a graphics context.
class SystemInfo(ctypes.Structure):
    _fields_ = [('name', ctypes.c_char_p), ('version', ctypes.c_char_p),
                ('extensions', ctypes.c_char_p), ('fullpath', ctypes.c_bool),
                ('block_extract', ctypes.c_bool)]
# Desktop Linux resolves core OpenGL entry points from the frontend's global
# GL library. Supply that host dependency for this standalone identity probe;
# retain eager symbol resolution so unrelated missing symbols still fail.
if sys.platform.startswith('linux'):
    host_gl = ctypes.CDLL(ctypes.util.find_library('GL') or 'libGL.so.1',
                          mode=ctypes.RTLD_GLOBAL)
lib = ctypes.CDLL(str(root/name))
lib.retro_get_system_info.argtypes = [ctypes.POINTER(SystemInfo)]
actual = SystemInfo()
lib.retro_get_system_info(ctypes.byref(actual))
assert actual.name == b'Supermodel' and actual.extensions == b'zip'
expected_version = re.search(r'^display_version = "([^"]+)"$', info, re.M)
assert expected_version and actual.version.decode() == expected_version[1], 'Runtime/metadata version mismatch'
print(f'PASS: {len(files)} package files; runtime identity, metadata, XML, licenses, provenance and hashes')
