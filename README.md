# coroutine

Interesting projects are 4 and 6

producer  is a coroutine which yields the digits of pi.


4 : consumer is a function,  which drives the producer with explicit  calls : [sequence diagram](https://github-lvs.corpzone.internalzone.com/jedwards/coroutine/blob/master/generator.diagram.txt)

5 : consumer is a coroutine,  which `co_await`s the producer : [sequence diagram](https://github-lvs.corpzone.internalzone.com/jedwards/coroutine/blob/master/co_await.diagram.txt)
