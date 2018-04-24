import os
import time
import sys
import git
import subprocess


def git_root(path):
    git_repo = git.Repo(path, search_parent_directories=True)
    git_root = git_repo.git.rev_parse("--show-toplevel")
    return git_root


def project_root():
    return "%s" % git_root("./")


class TsParser(object):
    """This object reflects TsParser executable."""
    parser = os.path.join(project_root(), 'build/tsparser')

    def __init__(self):
        self.proc = None

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

        self.proc = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                                     stderr=subprocess.PIPE,
                                     universal_newlines=True)
        out, err, exitcode = self.wait()

        print(out)
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
        out, err = self.proc.communicate(timeout=timeout * 1)
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
