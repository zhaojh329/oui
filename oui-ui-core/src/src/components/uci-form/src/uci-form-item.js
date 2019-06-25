export default {
  name: 'UciFormItem',
  inject: ['uciForm'],
  props: {
    sid: String,
    option: Object,
    table: Boolean
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
  },
  watch: {
    visible() {
      if (this.visible) {
        this.$nextTick(() => {
          this.uciForm.$refs['form'].validateField(this.prop);
        });
      } else {
        this.uciForm.validates[this.prop].valid = true;
      }
    }
  },
  render() {
    if (!this.visible)
      return '';

    const optView = this.option.view(this.prop, this.sid);
    const formItem = <el-form-item label={this.label} prop={this.prop} label-width={this.table ? 'auto' : ''}>{ optView }</el-form-item>;

    if (this.table)
      return formItem;

    return (
      <el-row gutter={10}>
        <el-col md={20} lg={this.lgForm} xl={this.xlForm}>{ formItem }</el-col>
        <el-col md={4} lg={this.lgDesc} xl={this.xlDesc}>{ this.option.description }</el-col>
      </el-row>
    );
  }
}
