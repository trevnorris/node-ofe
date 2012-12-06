Here's how to use:

```sh
npm install ofe
```

```javascript
require('ofe').call();
```

This will overwrite `OnFatalError` to create a heapdump when your app fatally crashes.

HUGE props to bnoordhuis for his node-buffertools for all the actually important code.
