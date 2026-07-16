# ===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ===----------------------------------------------------------------------===##

import os


def _getSubstitution(substitution, all_substitutions):
    for (orig, replacement) in all_substitutions:
        if orig == substitution:
            return replacement
    raise ValueError("Substitution {} is not in the config.".format(substitution))


def _appendToSubstitution(substitutions, key, value):
    return [(k, v + " " + value) if k == key else (k, v) for (k, v) in substitutions]


def _validateExpectedFeatures(config, lit_config, expected_features_file):
    """
    Validate that all features listed in the golden file are present in the
    actual configuration. This is used in CI to detect when features silently
    disappear, which would cause tests to be disabled without anyone noticing.

    This is a subset check: every feature in the golden file must be present,
    but the actual configuration may have additional features not listed in
    the file. This avoids brittleness from version-specific features like
    compiler versions or target triples.
    """
    note = lambda s: lit_config.note("({}) {}".format(config.name, s))

    # Resolve relative paths against the test source root
    if not os.path.isabs(expected_features_file):
        expected_features_file = os.path.join(
            config.test_source_root, expected_features_file
        )

    # Read expected features from the golden file
    with open(expected_features_file, "r") as f:
        expected = set(
            line.strip()
            for line in f
            if line.strip() and not line.strip().startswith("#")
        )

    actual = config.available_features
    missing = expected - actual

    if not missing:
        note(
            "Expected features validated successfully against '{}'".format(
                expected_features_file
            )
        )
        return

    # Build a clear error message
    lines = []
    lines.append(
        "Expected features missing (expected_features={}):\n".format(
            expected_features_file
        )
    )
    lines.append("  The following features were expected but not detected:")
    for f in sorted(missing):
        lines.append("    - {}".format(f))
    lines.append("")
    lines.append(
        "If this change is expected, update the golden file at '{}'.".format(
            expected_features_file
        )
    )

    lit_config.fatal("({}) {}".format(config.name, "\n".join(lines)))


def configure(parameters, features, config, lit_config):
    note = lambda s: lit_config.note("({}) {}".format(config.name, s))
    debug = lambda s: lit_config.dbg("({}) {}".format(config.name, s))
    config.environment = dict(os.environ)

    # Apply the actions supplied by parameters to the configuration first, since
    # parameters are things that we request explicitly and which might influence
    # what features are implicitly made available next.
    for param in parameters:
        actions = param.getActions(config, lit_config.params)
        for action in actions:
            action.applyTo(config)
            debug(
                "Applied '{}' as a result of parameter '{}'".format(
                    action.pretty(config, lit_config.params),
                    param.pretty(config, lit_config.params),
                )
            )

    # Then, apply the automatically-detected features.
    for feature in features:
        actions = feature.getActions(config)
        for action in actions:
            action.applyTo(config)
            debug(
                "Applied '{}' as a result of implicitly detected feature '{}'".format(
                    action.pretty(config, lit_config.params), feature.pretty(config)
                )
            )

    # Print the basic substitutions
    for sub in ("%{cxx}", "%{flags}", "%{compile_flags}", "%{link_flags}", "%{benchmark_flags}", "%{exec}"):
        note("Using {} substitution: '{}'".format(sub, _getSubstitution(sub, config.substitutions)))

    # Print all available features
    note("All available features: {}".format(", ".join(sorted(config.available_features))))

    # Validate expected features if a golden file was specified
    expected_features_file = lit_config.params.get(
        "expected_features", getattr(config, "expected_features", "")
    )
    if expected_features_file:
        _validateExpectedFeatures(config, lit_config, expected_features_file)
