# Lock-Free Single-Producer, Single-Consumer Queues

<img width="359" height="58" alt="Screenshot 2026-08-25 123356" src="https://github.com/user-attachments/assets/c5b24ba6-db68-4727-ad96-7ae5d4de02fd" />

This repo shows two nearly identical single-producer, single-consumer queues using acquire/release semantics. One queue caches the consumer offset for the producer, and the producer offset for the consumer, preventing some cache invalidation as both threads request each other's offsets. You can see the performance difference noted in the screenshot.

The benchmark produces and consumes 1,000,000 8-byte values 1000 times and takes the average. It also displays the total runtime.

###### Warning: This should not be treated as a real benchmark. I recreate threads on each run, include that time in the benchmark, and don't do any CPU-pinning. Still, this was a fun exercise.
