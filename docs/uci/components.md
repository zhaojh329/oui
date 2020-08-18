# Components

::: tip
The `self` appearing in the text indicates the current component instance itself.
:::

## oui-form

Represents a uci configuration file. Other components must be wrapped by this component.

### Attributes
| Name      | Description        | Type      | Accepted Values       | Default  |
|---------- |------------ |---------- |-------------|-------- |
| uci-config    | uci configuration file  | string   | — | — |

### Events
| Name   | Description         | Parameters   |
|---------- |------------- |---------- |
| applied   | triggers when apply configuration complete | — |

## oui-typed-section

Represents all uci sections which has the same type.

### Attributes
| Name      | Description        | Type      | Accepted Values       | Default  |
|---------- |------------ |---------- |-------------|-------- |
| type    | section type  | string   | — | — |
| title    | title  | string   | — | — |
| addremove   | Can be added or removed | boolean | — |  false |
| anonymous | Whether it is an anonymous section, if it is false, the add operation will add a named section  | boolean   | — |  true |
| columns     | table columns | array | — |  - |
| sortable  | Support for table sorting | boolean | — |  false |
| filter    | filter | Function(s) | — | — |
| collabsible | collabsible | boolean | — | true |
| teasers | The name of the option to display when collapsed (all options are displayed by default) | array | — | — |
| add | Custom add function. | Function(self) | — | — |

### 事件
| 事件名称   | 说明         | 回调参数   |
|---------- |------------- |---------- |
| change    | 当选项的值改变时触发 | (sid, self) |
| change-xx | 某个特定的选项的值改变时触发(其中xx表示具体的选项名称) | (self) |

### Filter

Suppose there is such a uci configuration file: test
```
config item
    option name qa
    option age 32
config item
    option name qa
    option age 18
```
Require only showing items older than 30:
``` vue
<template>
  <oui-form uci-config="test">
    <oui-typed-section type="item" :filter="filter">
    ...
    </oui-typed-section>
  </oui-form>
</template>

<script>
export default {
  methods: {
    filter(s) {
      return s.age > 30
    }
  }
}
</script>
```

### Custom add function

Returns the added section ID or Promise object.

Suppose there is such a uci configuration file: test
```
config item
    option name qa
    option age 32
```
The name option is required to be entered by the user when adding a section, and cannot be repeated, and cannot be modified once added.
``` vue
<template>
  <oui-form uci-config="test">
    <oui-typed-section type="item" :filter="filter" addremove :add="addItem" v-slot="{ s }">
      <oui-form-item-dummy label="Name" name="name":uci-section="s"/>
    </oui-typed-section>
  </oui-form>
</template>

<script>
export default {
  methods: {
    addItem(self) {
      this.$prompt({
        title: 'Add',
        placeholder: 'Please input a name',
        validator: value => {
          if (self.sections.filter(s => s.name === value).length > 0) {
            return 'The name already exist'
          }
        }
      }).then(value => {
        const sid = self.addSection()
        this.$uci.set('test', sid, 'name', value)
      })
    }
  }
}
</script>
```

## oui-named-section

Represents a uci section of a named uci section.

### Attributes
| Name      | Description        | Type      | Accepted Values       | Default  |
|---------- |------------ |---------- |-------------|-------- |
| Name      | section name  | string   | — | — |
| title     | title  | string   | — | — |

### Events
| Name   | Description         | Parameters   |
|---------- |------------- |---------- |
| change    | triggers when one option's value changed | (sid, self) |
| change-xx | Triggered when the value of a particular option changes(Where xx represents the specific option name) | (self) |

## oui-form-item

Used to customize UCI options

### Attributes
| Name        | Description        | Type      | Accepted Values       | Default  |
|------------ |------------ |---------- |-------------|-------- |
| uci-section | UCI object | object | - | - |
| label       | label | string | — | — |
| name        | option name (under the same section, must be unique) | string | — | — |
| help | a short description of the option(Support for HTML rendering) | string | — | — |
| required    | Required or not | boolean | — | false |
| initial     | initial value | string/number | — | — |
| depend      | depend | string | — | — |
| rules       | form validation rule | string/object/Function(value) | — | — |
| load        | Custom loading method | string/array/Function(self) | — | — |
| save        | Custom save function | Function(self) | — | — |

### Events
| Name   | Description         | Parameters   |
|---------- |------------- |---------- |
| change    | triggers when the option's value changed | self |

### Scoped Slots
| Name | Description     |
|------|----------|
| —    | Custom option content, the parameter is { self } |

### Custom option loading method

Implemented by providing the `load` attribute. This property supports three types of strings, arrays, and functions.
If you provide a function, you can return a value directly or a Promise object. If you supply a string or an array,
you can dynamically update the value of this option.
``` vue
<oui-form-item-switch label="X" name="x" :load="xEnabled" :uci-section="s"/>
...
<script>
export default {
  methods: {
    xEnabled() {
      return new Promise(resolve => {
        let en = false
        // TODO
        resolve(en);
      });
    }
  }
}
</script>  
```

### Custom option UI

``` vue
<oui-form-item label="名称" name="name" :uci-section="s">
  <template v-slot="{ self }">
    <span>{{ self.model }}</span>
  </template>
</oui-form-item>
```
Equivalent to
``` vue
<oui-form-item-dummy label="名称" name="name" :uci-section="s"/>
```

``` vue
<oui-form-item label="名称" name="name" :uci-section="s">
  <template v-slot="{ self }">
    <a-input v-model="self.model"/>
  </template>
</oui-form-item>
```
Equivalent to
``` vue
<oui-form-item-input label="名称" name="name" :uci-section="s"/>
```

## oui-form-item-dummy

Used only to display values, not for editing.

## oui-form-item-input

Used for editable uci options.

### Attributes
| Name        | Description        | Type      | Accepted Values       | Default  |
|------------ |------------ |---------- |-------------|-------- |
| placeholder | placeholder | string | — | — |
| password | toggleable password input | boolean | — | false |
| append | content to append after Input | string | — | — |

### content to append after Input

For example, some options require adding a unit after the input box.
![](./input_append.png)
``` vue
<oui-form-item-input label="Size" name="size" append="kiB" :uci-section="s"/>
```

## oui-form-item-switch

Used for options with a switch state.

### Attributes
| Name           | Description        | Type      | Accepted Values       | Default  |
|--------------- |------------ |---------- |-------------|-------- |
| initial        | initial value | boolean | — | — |
| true-value   | switch value when in on state | string/number/boolean | — | true |
| false-value | switch value when in off state | string/number/boolean | — | false |

## oui-form-item-select

Used when an option has multiple optional values.

### Attributes
| Name        | Description        | Type      | Accepted Values       | Default  |
|------------ |------------ |---------- |-------------|-------- |
| initial | initial value | string/array | — | — |
| options | optional list value | array | — | —  |
| multiple | whether multiple-select is activated | boolean | — | false |
| allow-create | whether creating new items is allowed | boolean | — | false |

## oui-form-item-dlist

Dynamic list. Corresponds to the list in the uci configuration.
