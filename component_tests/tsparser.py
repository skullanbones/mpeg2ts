#*****************************************************************
#
#    Copyright © 2017-2020 kohnech, lnwhome All rights reserved
#
#    mpeg2ts - mpeg2ts tsparser.py
#
#    This file is part of mpeg2ts (Mpeg2 Transport Stream Library).
#
#    Unless you have obtained mpeg2ts under a different license,
#    this version of mpeg2ts is mpeg2ts|GPL.
#    Mpeg2ts|GPL is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License as
#    published by the Free Software Foundation; either version 2,
#    or (at your option) any later version.
#
#    Mpeg2ts|GPL is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with mpeg2ts|GPL; see the file COPYING. If not, write to
#    the Free Software Foundation, 59 Temple Place - Suite 330,
#    Boston, MA 02111-1307, USA.
#
#******************************************************************/
import os
import time
import sys
import git
import subprocess
import fnmatch


def git_root(path):
    git_repo = git.Repo(path, search_parent_directories=True)
    git_root = git_repo.git.rev_parse("--show-toplevel")
    return git_root


def project_root():
    return "%s" % git_root("./")

def find(pattern, path):
    result = []
    for root, dirs, files in os.walk(path):
        for name in files:
            if fnmatch.fnmatch(name, pattern):
                result.append(os.path.join(root, name))
    return result

class TsParser(object):
    """This object reflects TsParser executable."""
    parser = find('tsparser-d', project_root())
    parser = parser[0]
    print("Using tsparser exe: {}".format(parser))

    def __init__(self, logger):
        self.proc = None
        self.log = logger

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        try:
            self.stop()
        except Exception as e:
            print("TsParser failed to stop: %r" % e)
        assert exc_type is None

    def start(self, **kwargs):
        """Start a process with arguments"""
        return self._start(**kwargs)

    def _start(self, **kwargs):
        """Internal function that forks a new process of executable with given
        arguments"""
        cmd = self.start_cmd(**kwargs)
        self.log.info("start command: %s" % cmd)

        self.proc = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                                     stderr=subprocess.PIPE,
                                     universal_newlines=True)
        out, err, exitcode = self.wait()

        #self.log.info(out)
        if err is not "":
            self.log.error(err)
        sys.stderr.write(err)

        return exitcode, out, err

    def start_cmd(self, **kwargs):
        args = self._collect_args(**kwargs)
        cmd = [self.parser]
        cmd.extend(args)
        return cmd

    def wait(self, timeout=60):
        """Wait for the process to exit"""
        print("Waiting for parser process to exit")
        wait_start_time = time.time()
        out, err = self.proc.communicate()
        print("Parser process done after ",
              round(time.time() - wait_start_time), " seconds")
        exitcode = self.proc.returncode
        return out, err, exitcode

    @classmethod
    def _collect_args(self, info=None, extra_args=None):
        """Collects all the input arguments for the process."""
        args = []

        if info:
            args.append('--info')
            args.append(info)
        if extra_args:
            args.extend(extra_args)

        args = (str(a) for a in args)
        return args
