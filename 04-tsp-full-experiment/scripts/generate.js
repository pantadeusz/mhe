/*
 * To generuje nam dowolnej wielkości zadanie. Wyświetla na standardowe wyjście błędów wynik którego się powinniśmy spodziewać. 
 */

let generate_circle = (n, R0) => {
    let problem = [];
    let R = R0 ? R0 : 100;
    let sorted = [];
    for (let i = 0; i < n; i++) {
        let a = Math.random() * Math.PI * 2.0; // angle
        let e = {
            x: Math.sin(a) * R,
            y: Math.cos(a) * R,
            name: `city${i}_${Math.ceil(Math.random() * 1000)}`
        };
        sorted.push({ a: a, v: e });
        problem.push(e);
    };

    return {
        q: { "problem": problem }, s: sorted.
            sort((a, b) => { return a.a - b.a; }).
            map(e => e.v).
            reduceRight(
                (a, b, i, arr) => { return a + Math.sqrt((arr[i].x - arr[(i + 1) % arr.length].x) ** 2 + (arr[i].y - arr[(i + 1) % arr.length].y) ** 2); }, 0.0)
    };
}

let R = 100;
let ret = generate_circle(process.argv[2]?parseFloat( process.argv[2] ):5);
console.log(JSON.stringify(ret.q, null, 4));
console.error(ret.s, 2 * Math.PI * R);
