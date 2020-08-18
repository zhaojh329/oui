# 组件

::: tip
文中出现的`self`均表示当前组件实例本身。
:::

## oui-form

代表一个uci配置文件。其它组件必须由该组件包裹。

### 属性
| 名称      | 说明        | 类型      | 可选值       | 默认值  |
|---------- |------------ |---------- |-------------|-------- |
| uci-config    | uci配置文件  | string   | — | — |

### 事件
| 事件名称   | 说明         | 回调参数   |
|---------- |------------- |---------- |
| applied     | 当应用配置完成时触发 | — |

## oui-typed-section

代表具有同一类型的uci section。

### 属性
| 名称      | 说明        | 类型      | 可选值       | 默认值  |
|---------- |------------ |---------- |-------------|-------- |
| type    | section类型  | string   | — | — |
| title    | 标题  | string   | — | — |
| addremove  | 是否可以添加或者删除 | boolean | — |  false |
| anonymous | 是否是匿名section，若为false，添加操作将添加一个命名section  | boolean   | — |  true |
| columns   | 表格列 | array | — |  - |
| expanded  | 表格扩展 | boolean | - | false |
| sortable  | 是支持表格排序 | boolean | — |  false |
| filter    | 过滤 | Function(s) | — | — |
| collabsible | 是否可折叠 | boolean | — | true |
| teasers | 折叠时需要显示的选项名称(默认显示所有选项) | array | — | — |
| add | 自定义添加函数。 | Function(self) | — | — |

### 事件
| 事件名称   | 说明         | 回调参数   |
|---------- |------------- |---------- |
| change    | 当选项的值改变时触发 | (sid, self) |
| change-xx | 某个特定的选项的值改变时触发(其中xx表示具体的选项名称) | (self) |

### 过滤

通过`filter`属性可实现过滤。该属性类型为一个函数，返回boolean值。

假设有这样一个uci配置文件：test
```
config item
    option name qa
    option age 32
config item
    option name qa
    option age 18
```
要求只展示年龄大于30的item:
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

### 自定义添加

通过`add`属性可实现自定义添加功能。该属性类型为一个函数，返回添加的section ID或者Promise对象。

假设有这样一个uci配置文件：test
```
config item
    option name qa
    option age 32
```
要求其中的name选项在添加section时由用户输入，并且不能重复，而且一旦添加就不能修改。
``` vue
<template>
  <oui-form uci-config="test">
    <oui-typed-section type="item" :filter="filter" addremove :add="addItem" v-slot="{ s }">
      <oui-form-item-dummy label="名称" name="name" :uci-section="s"/>
    </oui-typed-section>
  </oui-form>
</template>

<script>
export default {
  methods: {
    addItem(self) {
      this.$prompt({
        title: '添加',
        placeholder: '请输入名称',
        validator: value => {
          if (self.sections.filter(s => s.name === value).length > 0) {
            return '名称已存在'
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

代表具有某个命名的uci section。

### 属性
| 名称      | 说明        | 类型      | 可选值       | 默认值  |
|---------- |------------ |---------- |-------------|-------- |
| name    | section名称  | string   | — | — |
| title    | 标题  | string   | — | — |

### 事件
| 事件名称   | 说明         | 回调参数   |
|---------- |------------- |---------- |
| change    | 当选项的值改变时触发 | (sid, self) |
| change-xx | 某个特定的选项的值改变时触发(其中xx表示具体的选项名称) | (self) |

## oui-form-item

用于自定义 uci 选项

### 属性
| 名称        | 说明        | 类型      | 可选值       | 默认值  |
|------------ |------------ |---------- |-------------|-------- |
| uci-section | UCI 对象 | object | - | - |
| label       | 标签 | string | — | — |
| name        | 名称 | string | — | — |
| help | 对该选项的一个简短描述 | string | — | — |
| required    | 是否必填 | boolean | — | false |
| initial     | 初始值 | string/number | — | — |
| depend      | 依赖 | string | — | — |
| rules       | 表单验证规则 | string/object/Function(value) | — | — |
| load        | 自定义加载方式 | string/array/Function(self) | — | — |
| save        | 自定义保存函数 | Function(self) | — | — |

### 事件
| 事件名称   | 说明         | 回调参数   |
|---------- |------------- |---------- |
| change    | 当选项的值改变时触发 | self |

### 作用域插槽
| 名称 | 说明     |
|------|----------|
| —    |自定义选项内容，参数为 { self } |

### 自定义选项加载方式。

通过提供`load`属性实现。该属性支持字符串，数组，函数三种类型。如果提供一个函数，可直接返回一个值或者Promise对象。如果提供一个字符串或者数组，可动态更新该选项的值。
``` vue
<oui-form-item-switch label="测试" name="x" :load="xEnabled" :uci-section="s"/>
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

### 自定义选项UI

通过默认作用域插槽实现。
``` vue
<oui-form-item label="名称" name="name" :uci-section="s">
  <template v-slot="{ self }">
    <span>{{ self.model }}</span>
  </template>
</oui-form-item>
```
等价于
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
等价于
``` vue
<oui-form-item-input label="名称" name="name" :uci-section="s"/>
```

## oui-form-item-dummy

仅用于显示值，不能编辑。

## oui-form-item-input

用于可编辑的uci选项。

### 属性
| 名称        | 说明        | 类型      | 可选值       | 默认值  |
|------------ |------------ |---------- |-------------|-------- |
| placeholder | 占位符 | string | — | — |
| password | 密码框 | boolean | — | false |
| append | 输入框后置内容 | string | — | — |

### 输入框后置内容

比如某些选项需要在输入框后面添加一个单位。
![](./input_append.png)
``` vue
<oui-form-item-input label="系统日志缓冲区大小" name="log_size" append="kiB" :uci-section="s"/>
```

## oui-form-item-switch

用于具有开关状态的选项。

### 属性
| 名称           | 说明        | 类型      | 可选值       | 默认值  |
|-------------- |------------ |---------- |-------------|-------- |
| initial      | 初始值 | boolean | — | — |
| true-value   | switch打开时的值 | string/number/boolean | — | true |
| false-value  | switch关闭时的值 | string/number/boolean | — | false |

## oui-form-item-select

用于一个选项具有多个可选值的情况。

### 属性
| 名称        | 说明        | 类型      | 可选值       | 默认值  |
|------------ |------------ |---------- |-------------|-------- |
| initial | 初始值 | string/array | — | — |
| options | 可选列表值 | array | — | —  |
| multiple | 多选 | boolean | — | false |
| allow-create | 是否允许用户创建新条目 | boolean | — | false |

## oui-form-item-list

动态列表。对应uci配置里面的list。
