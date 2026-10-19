import http from 'k6/http';
import { check, sleep } from 'k6';

export const options = {
  vus: 12,
  duration: '40s',
  thresholds: {
    http_req_failed: ['rate<0.01'],
    http_req_duration: ['p(95)<350'],
  },
};

export default function () {
  const payload = JSON.stringify({
    title: `ramil-${__VU}-${__ITER}`,
    assignee: 'ramil',
    priority: 3,
    tag: 'load',
  });
  const created = http.post('http://localhost:8080/tasks', payload, {
    headers: { 'Content-Type': 'application/json' },
  });
  check(created, { '201': (r) => r.status === 201 });
  check(http.get('http://localhost:8080/tasks'), { '200': (r) => r.status === 200 });
  sleep(0.8);
}
