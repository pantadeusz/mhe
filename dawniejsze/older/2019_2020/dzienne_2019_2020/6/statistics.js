const fs = require('fs');

///// SETUP ///////////////////////////////////////////////////////////

// the folder containing experiment
let experiment_folders = [];
fs.readdirSync('./output/').forEach(series_name => {
	experiment_folders.push(`./output/${series_name}/`);
});

///////////////////////////////////////////////////////////////////////

// the statics - this will append to the current summary stats
const generate_summary_stats = (experiment_folder, summary_stats_zero) => {
	let summary_stats = summary_stats_zero?summary_stats_zero:{};
	// create the arrays structure to gather experiments data
	let summary_methods_list = [];
	let summary_experiment_input_sizes = [];
	let summary_test_numbers = [];
	
	// get experiment sets
	fs.readdirSync(experiment_folder).forEach(file => {
		if (/(.*)-(.*)-(.*)\.json/.test(file)) {
			var test = file.match(/(.*)-(.*)-(.*)\.json/);
			summary_methods_list.push(test[1]);
			summary_experiment_input_sizes.push(test[2]);
			summary_test_numbers.push(test[3]);
		}
	});
	
	// create object that will be handlling: summary_stats[method][experiment_size][experiment_index]
	summary_methods_list.forEach((method,i) => {
		summary_stats[method] = summary_stats[method]?summary_stats[method]:{};
		summary_experiment_input_sizes.forEach((experiment_input_size,i) => {
			summary_stats[method][experiment_input_size] = summary_stats[method][experiment_input_size]?summary_stats[method][experiment_input_size]:{};
		});
	});
	
	// get  the metadata for the experiment
	const file_to_metadata = (file) => {
			var test = file.match(/(.*)-(.*)-(.*)\.[tj][sx][ot].*/);
			const experiment_meta = {
				"method": test[1],
				"experiment_input_size": test[2],
				"i": test[3]
			};
			return experiment_meta;
	};
	
	// iterate over json files - get goal values
	fs.readdirSync(experiment_folder).forEach(file => {
		if (/(.*)-(.*)-(.*)\.json/.test(file)) {
			const experiment_meta = file_to_metadata(file);
			const experiment_data = JSON.parse(fs.readFileSync(`${experiment_folder}/${file}`, 'utf8'));
			const duration = parseFloat(fs.readFileSync(`${experiment_folder}/${experiment_meta.method}-${experiment_meta.experiment_input_size}-${experiment_meta.i}.txt`, 'utf8').split('\n').filter((l)=>{return /calculation_time/.test(l);})[0].split(' ')[2]);
			summary_stats[experiment_meta.method][experiment_meta.experiment_input_size][`${experiment_meta.i}_${Math.random()}`] = {goal:experiment_data.goal,duration:duration};
		}
	});
	
	return summary_stats;
};


let summary_stats;
experiment_folders.forEach((experiment_folder) => {summary_stats = generate_summary_stats(experiment_folder,summary_stats);});
console.log(experiment_folders);
console.log(`#method size goal diff_to_best time_s`);
for (let method_name in summary_stats) {
    for (let experiment_input_size in summary_stats[method_name]) {
        let s = 0.0;
        let duration = 0.0;
        let n = 0.0;
        for (let key in summary_stats[method_name][experiment_input_size]) {
			const v = summary_stats[method_name][experiment_input_size][key];
            if (v.goal) {
				s += v.goal;
				duration += v.duration;
				n++;
			}
        }
        let best_in_category = function(){
			for (let key in summary_stats["brute_force_find_solution"][experiment_input_size]) {
				return summary_stats["brute_force_find_solution"][experiment_input_size][key].goal;
			}
		}();
        console.log(`${method_name} ${experiment_input_size} ${n} ${(s / n).toFixed(5)} ${((s / n) / best_in_category - 1.0).toFixed(5)} ${(duration/n).toFixed(6)}`)
    }
}
