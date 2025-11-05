import http from 'k6/http';
import { sleep, check } from 'k6';

export const options = {
	stages: [
		{ duration: '10s', target: 20 },
		{ duration: '30s', target: 100 },
		{ duration: '10s', target: 200 },
		{ duration: '20s', target: 0 },
	],
	thresholds: {
		http_req_failed: ['rate<0.01'],
		http_req_duration: ['p(95)<400'],
	},
};

const TARGET = __ENV.K6_TARGET || 'http://127.0.0.1:8080/';

export default function () {
	const res = http.get(TARGET);
	check(res, {
		'status is 200': (r) => r.status === 200,
	});
	sleep(1);
}
