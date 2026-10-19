import http from 'k6/http';
import { check, sleep } from 'k6';
export const options = { vus: 18, duration: '25s' };
export default function () {
  check(http.get('http://localhost:8080/tasks'), { '200': (r) => r.status === 200 });
  sleep(0.25);
}
