# LNT tools for libc++ performance tracking

This directory contains utilities for continuous benchmarking of libc++ with LNT.

## Gathering historical performance data

When generating historical performance data, benchmarking every commit of libc++
is prohibitively expensive. Furthermore, generating this data from scratch is
expected  to be common, since it must happen whenever a fixed parameter like the
compiler or the OS changes.

Instead, the tools in this directory aim to make it possible to generate historical
performance data quickly with coarse granularity, with the goal of then generating
finer granularity performance data based on coarse granularity observations (e.g.
finding regressions between two distant commits). At this time, triggering finer
granularity data points is done manually.

Coarse grained performance data is obtained by determining "anchor commits", which
are libc++ commits that fall at specific intervals (e.g. the first commit of every
week). These anchor commits remain stable through time: they don't change as new
commits are introduced. This makes it possible to re-generate performance data for
the same anchor commits with a different configuration, and to compare across
configurations.

For coarse grained performance data, the system is meant to be run periodically on
a schedule. It keeps no state of its own: on every invocation, what should be measured
is recomputed from Git and what has been measured is recomputed from LNT and from the
Github Actions API. This makes the overall system converge towards a state where all
the desired commits have been benchmarked, without necessarily ever reaching it (as
new desired commits are added).

To achieve this, three tools work together:

```
# What should have benchmark data: one commit per week since 2023.
select-anchor-commits --since 2023-01-02 --every week > anchor-commits.txt

# What is missing from LNT (we want at least 3 samples for each commit).
plan-benchmarks --commit-list anchor-commits.txt                                \
                --lnt-url https://lnt.llvm.org --test-suite libcxx              \
                --machine <machine> --samples 3 > plan.jsonl

# Request the corresponding workflow runs, at most 4 at a time to be a good citizen.
export GITHUB_TOKEN=$(gh auth token)
dispatch-benchmarks --work-items plan.jsonl --max-in-flight 4 --dry-run
```

In a nutshell, `select-anchor-commits` produces the list of anchor commits that we want
data for. `plan-benchmarks` then looks at which commits we actually already have data for
in the LNT instance and produces a plan of what runs we need to trigger in order to
get data for the missing commits, taking into account the number of samples we want for
each commit. Finally, `dispatch-benchmarks` interprets this plan and actually dispatches
the Github workflows based on a budget, taking into account currently running workflows
and previously failed runs, if any (to avoid requesting runs that fail indefinitely).

Note that since `dispatch-benchmarks` both reads and creates workflow runs, it requires a
GitHub token with write access to the repository. That token can either be passed as an
argument or picked up from the `GITHUB_TOKEN` environment variable.

In production, this pipeline is run by the `libcxx-benchmark-cron.yml` workflow, which runs
it for each machine defined in `machines.json` on a schedule.

## Configuring the benchmark machines

`machines.json` describes the machines we benchmark on. It is the single source of truth
for both the workflow that runs the benchmarks (`libcxx-benchmark-commit.yml`) and the cron
that requests those runs (`libcxx-benchmark-cron.yml`). Each entry contains variables used
by the various workflows and the LNT machine name that the results will be reported under.

## Running benchmarks locally

On GitHub, the `libcxx-benchmark-commit.yml` workflow is used to run benchmarks and report
results to a LNT instance. This workflow wraps the `libcxx/utils/ci/lnt/run-benchmarks` script,
which can be used to benchmark locally:

```
run-benchmarks --test-suite-commit <SHA1> --machine <MACHINE>    \
               --compiler clang++ --benchmark-commit <SHA2>      \
               --output result.json
```

This will run the benchmarks (using the test suite at the specified `SHA1`) against libc++
as-of the specified `SHA2`, and produce a LNT-ready JSON report. The results can then be
submitted to a LNT instance if desired.

## Finding when a benchmark changed

The coarse-grained data gathered above tells us that a benchmark changed somewhere between
two anchor commits, but not where. `detect-changepoints` is what turns that data into such
an observation: given the history of a single benchmark, it reports the points at which it
changed level, which is the input to deciding where finer-grained data is worth gathering.

It knows nothing about LNT and reads its series as JSONL, one JSON object per measurement,
so the data can come from an LNT instance, from a local experiment, or from anywhere else.
Only a `value` field is required; `commit` labels the points and groups the measurements
taken at the same one, `ordinal` orders them, and any other field is ignored. That last part
is what lets the records an LNT instance produces be piped in as they are:

```
# Extract the history of one benchmark from a LNT instance.
curl -s -X POST "${LNT_URL}/api/v5/libcxx/query" -H 'Content-Type: application/json'    \
     -d "{\"machine\": \"${MACHINE}\", \"metric\": \"execution_time\",                  \
          \"test\": [\"${BENCHMARK}\"], \"limit\": 10000}"                              \
  | jq -c '.items[]' > series.jsonl

detect-changepoints --input series.jsonl --aggregate median
```

The changepoints come back on standard output in the same shape, one JSON object per line,
carrying the commits the change falls between, the level on either side of it, the relative
change and its p-value.

A commit that was benchmarked several times appears as several records, and `--aggregate` says
how to reduce them to the one value that stands for that commit. It has no default and is
required whenever the input holds repeats, because the alternative of treating them as
independent observations is wrong in a way that is invisible in the output: repeated runs of
one commit share a binary, so they resemble each other more than they resemble the runs of
the commit next door, and the permutation test reads that resemblance as structure. Left
alone it reports several times more changes than are really there.

A change is located between two consecutive *measured* commits, so with coarse-grained data
that interval spans everything that landed in between. Narrowing it down means benchmarking
commits inside the interval, which is what `plan-benchmarks` and `dispatch-benchmarks` above
are for.

See `detect-changepoints --help` for how the method works and for what the reported p-values
do and do not mean.

## Setting up a local LNT instance

```
# Create an instance and run a server
lnt create my-instance
echo "api_auth_token = 'example_token'" >> my-instance/lnt.cfg
lnt runserver my-instance

# In another terminal, create the libcxx test suite on the locally-running server
cat <<EOF > lnt-admin-config.yaml
lnt_url: "http://localhost:8000"
database: default
auth_token: example_token
EOF
lnt admin --config lnt-admin-config.yaml --testsuite libcxx test-suite add libcxx/utils/ci/lnt/schema.yaml

# Then submit to the local instance
submit-benchmarks --lnt-url http://localhost:8000 --test-suite libcxx result.json
```
