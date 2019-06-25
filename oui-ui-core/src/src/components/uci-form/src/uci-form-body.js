export default {
  name: 'UciFormBody',
  inject: ['uciForm'],
  render() {
    const sections = this.uciForm.sections;

    if (this.uciForm.tabbed) {
      const tabPanes = sections.map((s, i) => {
        const errNum = s.getErrorNum();
        return (
          <el-tab-pane name={i + ''}>
            <span slot="label">
              { s.title }{ errNum > 0 && <el-badge value={errNum} />}
            </span>
            {s.view()}
          </el-tab-pane>
        );
      });
      return <el-tabs>{ tabPanes }</el-tabs>;
    } else {
      return <div>{sections.map(s => <el-card style="margin-bottom: 10px" header={s.title}>{ s.view() }</el-card>)}</div>;
    }
  }
}
