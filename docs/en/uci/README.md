# Guide

Oui encapsulates some Vue components for uci, which is convenient for developers to call.

``` vue
<template>
  <uci-form config="test">
    <uci-section title="Test" name="main">
      <uci-option-input label="Name" name="name"></uci-option-input>
    </uci-section>
  </uci-form>
</template>
```
This is the basic structure of the UCI configuration page.

Once created, the `uci-form` component loads the UCI configuration file specified by the `config`
property by calling ubus. The form is then generated from the subcomponent.

## Form Validation

Form validation is implemented by specifying `rules` attribute to `uci-option-xx` components. This property supports strings, objects, and custom functions.

``` vue
<uci-option-input label="ID" name="id" rules="integer"></uci-option-input>
```
Equivalent to
``` vue
<uci-option-input label="ID" name="id" :rules="{type: 'integer'}"></uci-option-input>
```
Equivalent to
``` vue
<uci-option-input label="ID" name="id" :rules="validateID"></uci-option-input>
...
<script>
export default {
  methods: {
    validateID(value) {
      if (value === '')
        return;

      if (!isNaN(value) && parseInt(value).toString() === value)
        return;

      return this.$t('Must be a integer');
    }
  }
}
</script>
```

Multiple rules can be specified by object:
``` vue
<uci-option-input label="ID" name="id" :rules="{type: 'integer', min: 12, max: 100}"></uci-option-input>
```

If this option is mandatory, you can set the `required` attribute, which is of Boolean type and defaults to false.
``` vue
<uci-option-input label="ID" name="id" required></uci-option-input>
```

Oui currently supports validation rules:
- url
- email
- number
- integer
- uinteger
- min
- max
- hostname
- ip4addr
- ip6addr
- ipaddr
- netmask4
- netmask6
- host
- port
- macaddr
- uciname

## Option dependency

The display of options depends on the value of one or more other options.

Dependency can be implemented by setting the `depend` attribute to the `uci-option-xx` component. The type of this property is a string. We need to provide an expression in string.
``` js
depend="(a == 12 || a == 'x') && y == 4 && q != 5 && !z"
```

The `a` option depends on the `b` option. If the value of the `b` option is '5', the `a` option will be displayed, otherwise it will not be displayed:
``` vue
<uci-option-input label="A" name="a" depend="b == '5'"></uci-option-input>
<uci-option-input label="B" name="b"></uci-option-input>
```

The `a` option depends on the `b` option and the `c` option. If the value of the `b` option is 'oui', and the value of the `c` option
is not equal to 'u', the `a` option will be displayed, otherwise it will not be displayed:
``` vue
<uci-option-input label="A" name="a" depend="b == 'oui' && c != 'u'"></uci-option-input>
<uci-option-input label="B" name="b"></uci-option-input>
<uci-option-input label="C" name="c"></uci-option-input>
```

The `a` option depends on the `b` option and the `c` option. If the value of the `b` option is true and the value of the
`c` option is false, the a option will be displayed, otherwise it will not be displayed:
``` vue
<uci-option-input label="A" name="a" depend="b && !c"></uci-option-input>
<uci-option-switch label="B" name="b"></uci-option-switch>
<uci-option-switch label="C" name="c"></uci-option-switch>
```
