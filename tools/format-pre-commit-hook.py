#!/usr/bin/env python3
"""
Runs clang format, aborting the commit if the formatting is not OK.

Based on https://dx13.co.uk/articles/2015/4/3/Setting-up-git-clang-format.html
"""

import argparse
import subprocess
import sys
import os


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--install",
        help="Install this a pre-commit hook",
        action="store_true")

    return parser.parse_args()


def install():
    src = os.path.abspath(__file__)
    dst = os.path.join(
        os.path.dirname(src), '..', '.git', 'hooks', 'pre-commit')
    os.symlink(src, dst)


def main():
    args = parse_args()

    if args.install:
        install()
        return

    output = subprocess.check_output(["git", "clang-format", "--diff"])
    output = output.decode()

    if output.rstrip() not in [
            'no modified files to format',
            'clang-format did not modify any files'
    ]:
        print("Run make format, then commit.")
        sys.exit(1)


if __name__ == '__main__':
    main()
