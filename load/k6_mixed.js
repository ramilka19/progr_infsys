import http from 'k6/http';
import { check, sleep } from 'k6';
export const options = { vus: 14, duration: '25s' };
export default function () {
  if (__ITER % 2 === 0) {
    const body = JSON.stringify({ title: `x${__ITER}`, assignee: 'ramil', priority: 2 });
    check(http.post('http://localhost:8080/tasks', body, {
      headers: { 'Content-Type': 'application/json' },
    }), { '201': (r) => r.status === 201 });
  } else {
    check(http.get('http://localhost:8080/health'), { '200': (r) => r.status === 200 });
  }
  sleep(0.35);
}
