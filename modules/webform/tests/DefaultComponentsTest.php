<?php

namespace Drupal\form_builder_webform;

/**
 * Test the default component configuration of form builder element types.
 */
class DefaultComponentsTest extends \DrupalUnitTestCase {

  /**
   * Test that the default markup has a text-format.
   */
  public function testMarkupTextFormat() {
    $types = \FormBuilderLoader::instance()->getElementTypeInfo('webform', NULL);
    $component = $types['markup']['default']['#webform_component'];
    $element = webform_component_invoke($component['type'], 'render', $component);
    $this->assertNotEmpty($element['#format']);
  }

}
