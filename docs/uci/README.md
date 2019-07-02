# uci

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
