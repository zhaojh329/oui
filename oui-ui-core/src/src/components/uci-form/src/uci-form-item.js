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
      return !this.option.hide && this.option.visible(this.sid);
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

    return (
      <el-form-item prop={this.prop} label-width={this.table ? 'auto' : ''} style="max-width: 700px">
        {
          this.label &&
          <span slot="label">
            {this.label}
            {
              this.option.description &&
              <el-tooltip placement="top">
                <div slot="content" domPropsInnerHTML={this.option.description} />
                <i class="iconfont iconhelp" style="color: #3980DE" />
              </el-tooltip>
            }
          </span>
        }
        { this.option.view(this.prop, this.sid) }
      </el-form-item>
    );
  }
}
