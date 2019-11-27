const series_name = `2019-10-24`

const experiment_folder = `./output/${series_name}/`;
const fs = require('fs');


let summary_stats = {};
fs.readdirSync(experiment_folder).forEach(file => {
    if (/(.*)-(.*)-(.*)\.json/.test(file)) {
        var test = file.match(/(.*)-(.*)-(.*)\.json/);
        const experiment_meta = {
            "method": test[1],
            "experiment_input_size": test[2],
            "i": test[3]
        };
        experiment_data = JSON.parse(fs.readFileSync(`${experiment_folder}/${file}`, 'utf8'));
        if (summary_stats[experiment_meta.method]) {
            if (summary_stats[experiment_meta.method][experiment_meta.experiment_input_size]) {
                summary_stats[experiment_meta.method]
                [experiment_meta.experiment_input_size].push(experiment_data.goal);
            }
            else {
                summary_stats[experiment_meta.method][experiment_meta.experiment_input_size] =
                    [experiment_data.goal]
            }
        } else {
            summary_stats[experiment_meta.method] = {}
            summary_stats[experiment_meta.method] = {}
            summary_stats[experiment_meta.method][experiment_meta.experiment_input_size] =
                [experiment_data.goal]
        }
        //console.log(experiment_meta);
    }
});
console.log(summary_stats)
for (let method_name in summary_stats) {
    for (let experiment_input_size in summary_stats[method_name]) {
        let s = 0;
        let n = 0;
        for (v of summary_stats[method_name][experiment_input_size]) {
            s += v;
            n++;
        }
        console.log(`${method_name} ${experiment_input_size} ${s / n} ${(s / n) / summary_stats["brute_force_find_solution"][experiment_input_size][0] - 1.0}`)
    }
}
