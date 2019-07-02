# 指南

oui针对uci封装了一些vue组件，方便开发人员调用。

``` vue
<template>
  <uci-form config="test">
    <uci-section title="Test" name="main">
      <uci-option-input label="Name" name="name"></uci-option-input>
    </uci-section>
  </uci-form>
</template>
```
这是一个uci配置页面的基本结构。

`uci-form`组件在创建后，会通过调用ubus加载`config`属性指定的uci配置文件。然后根据子组件生成表单。

## 表单验证

通过给`uci-option-xx`组件指定`rules`属性实现表单验证。该属性支持字符串，对象以及自定义函数。

``` vue
<uci-option-input label="ID" name="id" rules="integer"></uci-option-input>
```
等价于
``` vue
<uci-option-input label="ID" name="id" :rules="{type: 'integer'}"></uci-option-input>
```
等价于
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

通过对象方式可以指定多个规则：
``` vue
<uci-option-input label="ID" name="id" :rules="{type: 'integer', min: 12, max: 100}"></uci-option-input>
```

如果该选项为必填项，可设置`required`属性，该属性类型为布尔值，默认为false。
``` vue
<uci-option-input label="ID" name="id" required></uci-option-input>
```

oui目前支持的验证规则：
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

## 选项依赖

选项的显示与否依赖于一个或者多个其它选项的值。

给`uci-option-xx`组件设置`depend`属性可以实现依赖。该属性的类型为字符串。我们需要提供一个以字符串表示的表达式。
``` js
depend="(a == 12 || a == 'x') && y == 4 && q != 5 && !z"
```

a选项依赖于b选项，如果b选项的值为'5'，a选项则显示，否则不显示：
``` vue
<uci-option-input label="A" name="a" depend="b == '5'"></uci-option-input>
<uci-option-input label="B" name="b"></uci-option-input>
```

a选项依赖于b选项和c选项，如果b选项的值为'oui'，而且c选项的值不等于'u'，a选项则显示，否则不显示：
``` vue
<uci-option-input label="A" name="a" depend="b == 'oui' && c != 'u'"></uci-option-input>
<uci-option-input label="B" name="b"></uci-option-input>
<uci-option-input label="C" name="c"></uci-option-input>
```

a选项依赖于b选项和c选项，如果b选项的值为真，而且c选项的值为假，a选项则显示，否则不显示：
``` vue
<uci-option-input label="A" name="a" depend="b && !c"></uci-option-input>
<uci-option-switch label="B" name="b"></uci-option-switch>
<uci-option-switch label="C" name="c"></uci-option-switch>
```