<?php

class FormBuilderFormBaseTest extends DrupalUnitTestCase {

  public static function getInfo() {
    return array(
      'name' => 'FormBuilderFormBase unit tests.',
      'description' => 'Tests form element handling.',
      'group' => 'Form builder',
    );
  }

  protected function emptyForm() {
    return new FormBuilderFormBase('webform', 'test', NULL, [], [], NULL);
  }

  /**
   * @covers FormBuilderFormBase::setElementArray
   */
  public function testSetElementArray() {
    $form = $this->emptyForm();
    $a['#form_builder']['element_id'] = 'A';
    $a['#key'] = 'a';
    $a['#type'] = 'textfield';
    $this->assertEqual('A', $form->setElementArray($a));
    $rform = $form->getFormArray();
    $this->assertArrayHasKey('a', $rform);

    $a['#key'] = 'x';
    $this->assertEqual('A', $form->setElementArray($a));
    $rform = $form->getFormArray();
    $this->assertArrayNotHasKey('a', $rform);
    $this->assertArrayHasKey('x', $rform);

    $b['#key'] = 'b';
    $b['#type'] = 'textfield';
    $b['#form_builder'] = ['element_id' => 'B', 'parent_id' => 'A'];
    $this->assertEqual('B', $form->setElementArray($b));
    $this->assertArrayNotHasKey('b', $form->getFormArray());
    $this->assertArrayHasKey('b', $form->getElementArray('A'));

    $b['#form_builder']['parent_id'] = 'NON EXISTING';
    $this->assertFalse($form->setElementArray($b));
    $this->assertArrayHasKey('b', $form->getElementArray('A'));

    $b['#form_builder']['parent_id'] = FORM_BUILDER_ROOT;
    $this->assertEqual('B', $form->setElementArray($b));
    $this->assertArrayHasKey('b', $form->getFormArray());
    $this->assertArrayNotHasKey('b', $form->getElementArray('A'));
  }

  /**
   * @covers FormBuilderFormBase::indexElementIds
   */
  public function testElementIdIndexing() {
    $form['a']['#type'] = 'textfield';
    $form['a']['#form_builder'] = ['element_id' => 'A'];
    $form_obj =  new FormBuilderFormBase('webform', 'test', NULL, [], $form);
    $this->assertNotEmpty($form_obj->getElementArray('A'));
  }
}
