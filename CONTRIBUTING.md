# Welcome Contributing to MPEG2TS

We love your input! We want to make contributing to this project as easy and transparent as possible, whether it's:

* Reporting a bug
* Discussing the current state of the code
* Submitting a fix
* Proposing new features
* Becoming a maintainer

## We Develop with Github

We use github to host code, to track issues and feature requests, as well as accept pull requests.

Pull requests are the best way to propose changes to the codebase (we use Git Flow as a branch strategy). We actively welcome your pull requests:

* Fork the repo and create your branch from `develop`.
* If you've added code that should be tested, add tests.
* If you've changed APIs, update the documentation.
* Ensure the test suite passes.
* Make sure your code lints and formats (clang-tidy).
* Issue the pull request!


## We Use Git Flow
Most important main branches are:

* **develop** (continous development)
* **master** (stores releases)

Along these there are a number of supporting branches:

* feature branches
* release branches
* hotfix branches

For more information about Git flow, please check [here](https://nvie.com/posts/a-successful-git-branching-model/).

## Issues
We use GitHub issues to track public bugs. Report a bug by opening a new issue; it's that easy!
Write bug reports with detail, background, and sample code

This is an example of a bug report.
Great Bug Reports tend to have:

    A quick summary and/or background
    Steps to reproduce
        Be specific!
        Give sample code if you can. My stackoverflow question includes sample code that anyone with a base R setup can run to reproduce what I was seeing
    What you expected would happen
    What actually happens
    Notes (possibly including why you think this might be happening, or stuff you tried that didn't work)

## Code style
Format and lint the C++ code using clang. We use C++ core guidelines as [style](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).


    make clang-format
    make clang-tidy

## License
Any contributions you make will be under the MPEG2TS GPL v2 License (see License).

In short, when you submit code changes, your submissions are understood to be under the same MPEG2TS GPL v2 that covers the project. Feel free to contact the maintainers if that's a concern. Report bugs using Github's issues.


## References

This document was adapted from the open-source contribution guidelines for Facebook's Draft