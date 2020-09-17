Here's how to use:

```sh
npm install ofe
```

```javascript
require('ofe').call();
```

This will overwrite `OnFatalError` to create a heapdump when your app fatally crashes.

Can also call this to trigger the heapdump manually and exit the process.

```js
require('ofe').trigger();
```
