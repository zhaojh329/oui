<template>
  <el-row :gutter="10" v-if="visible">
    <el-col :md="20" :lg="lgForm" :xl="xlForm">
      <el-form-item ref="form-item" :label="label" :prop="prop" :label-width="table ? 'auto' : ''">
        <uci-option-ui :sid="sid" :option="option"></uci-option-ui>
      </el-form-item>
    </el-col>
    <el-col :md="4" :lg="lgDesc" :xl="xlDesc">{{ option.description }}</el-col>
  </el-row>
</template>

<script>

import UciOptionUi from './uci-option-ui'

export default {
  name: 'UciFormItem',
  inject: ['uciForm'],
  props: {
    sid: String,
    option: Object,
    table: Boolean
  },
  components: {
    UciOptionUi
  },
  computed: {
    label() {
      if (this.table)
        return undefined;
      return this.option.label;
    },
    prop() {
      return this.option.formProp(this.sid);
    },
    visible() {
      const depend = this.option.parsedDepend;
      if (!depend)
        return true;

      let expr = depend.expr;

      depend.names.forEach(name => {
        const o = this.option.uciSection.children[name];
        if (!o)
          return false;
        let v = o.dependExprValue(this.sid);
        expr = expr.replace(new RegExp(name, 'gm'), v);
      });

      return eval(expr);
    },
    inDialog() {
      return this.$getParent('ElDialog');
    },
    lgForm() {
      if (this.inDialog)
        return 20;
      return 14;
    },
    lgDesc() {
      if (this.inDialog)
        return 4;
      return 10;
    },
    xlForm() {
      if (this.inDialog)
        return 14;
      return 11;
    },
    xlDesc() {
      if (this.inDialog)
        return 10;
      return 13;
    }
  }
}
</script>
