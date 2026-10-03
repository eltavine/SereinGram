"""Git settings for the tool tests that create repositories."""

import os
import unittest
from unittest import mock


def disable_background_maintenance():
    """Keeps git from leaving maintenance running in the module's repositories.

    Commit, merge and fetch start `git maintenance run --auto --detach`, which
    can still hold files under .git when a test removes its temporary directory.
    """
    count = int(os.environ.get("GIT_CONFIG_COUNT", "0"))
    patcher = mock.patch.dict(
        os.environ,
        {
            "GIT_CONFIG_COUNT": str(count + 1),
            f"GIT_CONFIG_KEY_{count}": "maintenance.auto",
            f"GIT_CONFIG_VALUE_{count}": "false",
        },
    )
    patcher.start()
    unittest.addModuleCleanup(patcher.stop)
