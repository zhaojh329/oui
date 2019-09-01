import mixin from './uci-option'
import Filebrowser from './filebrowser'

export default {
  name: 'UciOptionDummy',
  mixins: [mixin],
  components: {
    Filebrowser
  },
  methods: {
    showFileDialog(prop) {
      const h = this.$createElement;
      let file;

      this.$msgbox({
        title: this.$t('Select File'),
        message: h('Filebrowser', {
          on: {
            selected: selected => {
              file = selected;
            }
          }
        }),
        showCancelButton: true,
        beforeClose: (action, instance, done) => {
          delete instance.$slots.default;
          done();
        }
      }).then(() => {
        this.form[prop] = file;
      });
    },
    view(prop) {
      return (
        <el-input v-model={this.form[prop]} clearable>
          { <el-button style="background: #3a8ee6; color: white" slot="append" onclick={() => this.showFileDialog(prop)}>{ this.$t('Select...') }</el-button> }
        </el-input>
      );
    }
  }
}
