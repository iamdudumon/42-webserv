import http from 'k6/http';
import { sleep, check } from 'k6';

export const options = {
	stages: [
		{ duration: '20s', target: 30 },
		{ duration: '40s', target: 120 },
		{ duration: '20s', target: 200 },
		{ duration: '20s', target: 0 },
	],
	thresholds: {
		http_req_failed: ['rate<0.03'],
		http_req_duration: ['p(95)<800'],
	},
};

const BASE_URL = __ENV.K6_TARGET || 'http://127.0.0.1:8080/';
const GET_URL = `${BASE_URL}/`;
const UPLOAD_URL = `${BASE_URL}/cgi-bin/upload.py`;

const POST_RATIO = Math.min(Math.max(Number(__ENV.K6_POST_RATIO || 0.7), 0), 1);
const FILE_BYTES = Math.max(Number(__ENV.K6_FILE_BYTES || 256 * 1024), 1);
const SLEEP_SECONDS = Math.max(Number(__ENV.K6_SLEEP_SECONDS || 1), 0);

const FILE_BODY = 'x'.repeat(FILE_BYTES);

export default function () {
	const iterationTag = `${__VU}-${__ITER}`;
	if (Math.random() < POST_RATIO) {
		const payload = {
			file: http.file(FILE_BODY, `k6_upload_${iterationTag}.txt`, 'text/plain'),
		};
		const res = http.post(UPLOAD_URL, payload);
		check(res, {
			'post status 2xx': (r) => r.status >= 200 && r.status < 300,
		});
	} else {
		const res = http.get(GET_URL);
		check(res, {
			'get status 200': (r) => r.status === 200,
		});
	}
	sleep(SLEEP_SECONDS);
}
