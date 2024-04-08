from .echo import echo
from .exceptions import ClientError

def print_benchmark_reports(your, reference):
    your = your["benchmarks"]
    reference = reference["benchmarks"]

    echo.echo("Benchmarks (reference / your):")

    for ref_benchmark in reference:
        benchmark_name = ref_benchmark["name"]

        your_benchmark = None
        for benchmark in your:
            if benchmark["name"] == benchmark_name:
                your_benchmark = benchmark
                break

        if not your_benchmark:
            raise ClientError("Benchmark not found: {}".format(ref_benchmark["name"]))

        time_unit = ref_benchmark["time_unit"]

        your_real_time = your_benchmark["real_time"]
        ref_real_time = ref_benchmark["real_time"]

        echo.write("{}:\t{:0.2f} {} / {:0.2f} {}".format(benchmark_name, ref_real_time, time_unit, your_real_time, time_unit))
    
