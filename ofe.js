const binding = require('bindings')('ofe.node');
module.exports = { call: binding.call, trigger: binding.trigger };
