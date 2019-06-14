<template>
  <el-form-item ref="form-item" :label="label" :prop="prop" v-if="visible" :label-width="table ? 'auto' : ''">
    <el-input v-if="type === 'input'" v-model="form[prop]" :placeholder="option.placeholder"></el-input>
    <el-input v-else-if="type === 'dummy'" :value="form[prop]" readonly></el-input>
    <el-switch v-else-if="type === 'switch'" v-model="form[prop]" :active-value="option.activeValue" :inactive-value="option.inactiveValue"></el-switch>
    <el-select v-else-if="type === 'list'" v-model="form[prop]" :clearable="!option.required" :multiple="option.multiple" filterable style="width: 100%">
      <el-option v-for="oo in option.transformedOptions" :key="oo[0]" :label="oo[1] || oo[0]" :value="oo[0]"></el-option>
    </el-select>
    <uci-dlist v-else-if="type === 'dlist'" v-model="form[prop]" :prop="prop"></uci-dlist>
  </el-form-item>
</template>

<script>
import UciDlist from './uci-dlist'

export default {
  name: 'UciFormItem',
  props: {
    sid: String,
    option: Object,
    form: Object,
    table: Boolean
  },
  components: {
    UciDlist
  },
  computed: {
    label() {
      if (this.table)
        return undefined;
      return this.option.label;
    },
    prop() {
      return this.option.prop(this.sid);
    },
    type() {
      return this.option.type;
    },
    visible() {
      const depend = this.option.parsedDepend;
      if (!depend)
        return true;

      let expr = depend.expr;

      depend.names.forEach(name => {
        const o = this.option.uciSection.findOption(name);
        if (!o)
          return false;
        let v = o.formValue(this.sid);
        if (o.type === 'switch') {
          if (v === o.activeValue)
            v = 'true';
          else
            v = 'false';
        } else {
          v = `'${v}'`;
        }

        expr = expr.replace(new RegExp(name, 'gm'), v);
      });

      return eval(expr);
    }
  }
}
</script>
