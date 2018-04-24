import pytest

@pytest.fixture
def parser():
    """Get access to the TsParser binary to allow running it for
    testing or to get the command to start it."""
    from .tsparser import TsParser
    return TsParser()


def func(x):
    return x + 1

def test_answer():
    assert func(3) == 4

def test_help_message(parser):
    """Verify that help option is displayed"""
    out = parser.start(extra_args=['--help'])
    assert "Ts-lib simple command-line:" in out[1], "No help text in output"
    assert "USAGE: ./tsparser [-h] [-w PID] [-i PID] [-l log-level]" in out[1], "Wrong help output"
